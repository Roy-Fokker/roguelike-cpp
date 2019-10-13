require.config({ paths: { 'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.18.0/min/vs/' }});

window.MonacoEnvironment = {
  getWorkerUrl: function(workerId, label) {
    return `data:text/javascript;charset=utf-8,${encodeURIComponent(`
      self.MonacoEnvironment = {
        baseUrl: 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.18.0/min/'
      };
      importScripts('https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.18.0/min/vs/base/worker/workerMain.js');`
    )}`;
  }
};

function HSVtoRGB(h, s, v) {
  var r, g, b, i, f, p, q, t;
  i = Math.floor(h * 6);
  f = h * 6 - i;
  p = v * (1 - s);
  q = v * (1 - f * s);
  t = v * (1 - (1 - f) * s);
  switch (i % 6) {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
  }
  return {
    r: Math.round(r * 255),
    g: Math.round(g * 255),
    b: Math.round(b * 255)
  };
}

function CreatePastels(n, theme = 'dark', alpha = 1) {
  var colors = [];
  for (var i = 0; i < n; ++i) {
    var interpolation = (i / n) * 1;
    var color = theme == 'light' ? HSVtoRGB(interpolation, 0.3, 1) : HSVtoRGB(interpolation, 0.3, 0.8);
    colors.push(`rgba(${color.r},${color.g},${color.b},${alpha})`);
  }
  return colors;
}
const colors = CreatePastels(10, 'dark', 0.5);
const fontColors = CreatePastels(10);

let styles = '';
colors.forEach((color, index) => {
  styles += `.color${index} { background-color: ${color}; }`;
  styles += `.linecolor${index} { background: linear-gradient(to right, transparent var(--gradient-distance), ${color}); }`;
});
const style = document.createElement("style");
style.textContent = styles;
document.head.appendChild(style);

customElements.define('ui-code', class extends HTMLElement {
  constructor() {
    super();
    this.attachShadow({mode: 'open'});
    this.shadowRoot.innerHTML = `<style>
      :host {
        display: inline-flex;
        flex-flow: row nowrap;
        border: 1px solid gray;
      }
      ::slotted(.code) {
        flex: 0 0 auto;
        width: 800px;
      }
      :host > .comments {
        flex: 0 0 auto;
        width: 800px;
        display: flex;
        flex-flow: column nowrap;
        border-left: 1px solid gray;
        background-color: #1e1e1e;
        color: white;
        overflow-y: hidden;
        overflow-x: auto;
      }
      :host > .comments > div {
        font-family: Consolas,"Courier New", monospace;
        font-weight: normal;
        font-size: 14px;
        line-height: 19px;
        letter-spacing: 0px;
        white-space: pre;
        padding: 0 5px;
      }
      :host > .comments > div:empty:before {
        content: ' ';
        white-space: pre;
      }
    </style>
    <slot></slot>
    <div class="comments"></div>`;
  }
  connectedCallback() {
    let colorIndex = 0;
    function getColor()
    {
      const index = colorIndex++ % colors.length;
      return [`color${index}`, `linecolor${index}`, fontColors[index]];
    }
    
    require(['vs/editor/editor.main'], () => {
      (async () => {
      if (this.hasAttribute('commentWidth')) {
        this.shadowRoot.querySelector('.comments').style.width = `${this.getAttribute('commentWidth')}px`;
      }
      const url = this.getAttribute('url');
      // Find #L1-L2
      const lineMatches = url.match(/^.*?#L([0-9]*)(?:-L([0-9]*))?$/);
      let start = null;
      let end = null;
      if (lineMatches != null && lineMatches.length >= 2) {
        start = parseInt(lineMatches[1], 10);
        if (lineMatches.length == 3) {
          end = parseInt(lineMatches[2], 10);
        }
      }
      const code = await (await fetch(url)).text();
      let lines = code.split(/\n/g).map((line, index) => {
        const codeMatches = line.match(/^(.*?)(?:\s*\/\/.*)?$/);
        const commentMatches = line.match(/^.*?\/\/\s?(.*)$/);
        return {
          number: index + 1,
          code: codeMatches != null && codeMatches.length != 1 ? codeMatches[1] : '',
          comment: commentMatches != null && commentMatches.length != 1 ? commentMatches[1] : '',
          get isCodeEmpty() { return /^\s*$/.test(this.code); },
          get isCommentEmpty() { return /^\s*$/.test(this.comment); }
        };
      });
      if (lines.length > 2 && /\/\*\s*?SPDX-License-Identifier: MIT\s*?\*\//i.test(lines[0].code)) {
        lines.splice(0, lines[1].isCodeEmpty && lines[1].isCommentEmpty ? 2 : 1);
      }
      for (let i = lines.length - 1; i > 0; --i) {
        // Move comment only lines to the next line if there isn't a comment there already
        if (lines[i].isCodeEmpty && !lines[i].isCommentEmpty && lines[i + 1].isCommentEmpty) {
          lines[i + 1].comment = lines[i].comment;
          lines[i].comment = '';
        }
      }
      for (let i = 0; i < lines.length - 1; ++i) {
        if (lines[i].isCodeEmpty && lines[i].isCommentEmpty && lines[i + 1].isCodeEmpty && lines[i + 1].isCommentEmpty) {
          lines.splice(i--, 1);
        }
      }
      while (lines.length > 0 && lines[lines.length - 1].isCodeEmpty && lines[lines.length - 1].isCommentEmpty) {
        lines.pop();
      }
      while (start != null && lines.length > 0 && lines[0].number < start) {
        lines.shift();
      }
      while (end != null && lines.length > 0 && lines[lines.length - 1].number > end) {
        lines.pop();
      }
      const $code = document.createElement('div');
      $code.classList.add('code');
      this.appendChild($code);
      if (this.hasAttribute('width')) {
        $code.style.width = `${this.getAttribute('width')}px`;
      }
      const editor = monaco.editor.create($code, {
        value: lines.map(line => line.code).join('\n'),
        language: 'cpp',
        minimap: {
          enabled: false
        },
        hover: { delay: 0, enabled: true },
        readOnly: true,
        scrollbar: {
          vertical: 'hidden'
        },
        scrollBeyondLastLine: false,
        overviewRulerBorder: false,
        folding: false,
        renderLineHighlight: false,
        matchBrackets: false,
        occurrencesHighlight: false,
        selectionHighlight: false,
        theme: 'vs-dark',
        lineNumbers: originalLineNumber => lines[originalLineNumber - 1].number
      });
      const declarations = [];
      for (let i = 0; i < lines.length; ++i) {
        const comment = lines[i].comment;
        const [colorClass, lineColorClass, color] = getColor();
        const $comment = document.createElement('div');
        $comment.textContent = comment;
        $comment.style.color = color;
        this.shadowRoot.querySelector('.comments').appendChild($comment);
        if (!lines[i].isCommentEmpty) {
          declarations.push({
            range: new monaco.Range(i + 1,1,i + 1,1000), options:
            {
              isWholeLine: true,
              //linesDecorationsClassName: colorClass,
              className: lineColorClass,
              hoverMessage: comment
            }
          });
        }
      }
      editor.deltaDecorations([], declarations);
      const updateLines = () => {
        const el = editor.getDomNode();
        const viewOverlays = el.querySelectorAll('.view-overlays > div');
        let maximumWidth = 0;
        // editor.getScrollWidth() doesn't return the real scrollWidth, so improvise
        el.querySelectorAll('.view-line').forEach(node => {
          maximumWidth = Math.max(maximumWidth, node.querySelector('span').offsetWidth);
        });
        maximumWidth = Math.max(maximumWidth, editor.getScrollWidth());
        el.querySelectorAll('.view-line').forEach((node, index) => {
          viewOverlays[index].style.setProperty('--gradient-distance', `${(node.querySelector('span').offsetWidth / maximumWidth * 100).toFixed(2)}%`);
        });
      };
      editor.onDidChangeCursorPosition((e) => {
        setTimeout(updateLines, 0);
        setTimeout(updateLines, 100);
      });
      editor.onDidChangeCursorSelection(() => {
        setTimeout(updateLines, 0);
        setTimeout(updateLines, 100);
      });
      
      const el = editor.getDomNode();
      el.style.height = 0;
      editor.layout();
      let height = editor.getScrollHeight();
      el.style.width = el.parentNode.style.width;
      el.style.height = `${height + 7}px`;
      editor.layout();
      updateLines();
      })();
    });
  }
});