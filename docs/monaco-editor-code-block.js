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
        start = parseInt(lineMatches[1], 10) - 1;
      }
      if (lineMatches != null && lineMatches.length == 3) {
        end = parseInt(lineMatches[2], 10) - 1;
      }
      let code = await (await fetch(url)).text();
      let codeLines = code.split(/\n/g).map(line => {
        const matches = line.match(/^(.*?)(?:\s*\/\/.*)?$/);
        return matches != null && matches.length != 1 ? matches[1] : '';
      });
      //console.log(codeLines);
      let commentLines = code.split(/\n/g).map(line => {
        const matches = line.match(/^.*?\/\/\s?(.*)$/);
        return matches != null && matches.length != 1 ? matches[1] : '';
      });
      //console.log(commentLines);
      if (start !== null) {
        codeLines = codeLines.slice(start, end ? end + 2 : start + 1);
        commentLines = commentLines.slice(start, end ? end + 2 : start + 1);
      }
      const lineMap = {};
      codeLines.forEach((line, index) => {
        lineMap[index + 1] = index + 1 + (start ? start : 0);
      });
      if (codeLines.length > 2 && /\/\*\s*?SPDX-License-Identifier: MIT\s*?\*\//i.test(codeLines[0])) {
        const deleteNextLine = codeLines[1] == '';
        codeLines.splice(0, deleteNextLine ? 2 : 1);
        commentLines.splice(0, deleteNextLine ? 2 : 1);
        for (let i = 0; i < codeLines.length; ++i) {
            lineMap[i + 1] += deleteNextLine ? 2 : 1;
          }
      }
      for (let i = 0; i < codeLines.length - 1; ++i) {
        if (codeLines[i] == '' && commentLines[i] != '' && commentLines[i + 1] == '') {
          commentLines[i + 1] = commentLines[i];
          commentLines[i] = '';
        }
        if (/^\s*$/.test(codeLines[i]) && commentLines[i] == '' && /^\s*$/.test(codeLines[i + 1])) {
          codeLines.splice(i, 1);
          commentLines.splice(i, 1);
          for (let j = i; j < codeLines.length; ++j) {
            lineMap[j + 2]++;
          }
          i--;
        }
      }
      while (codeLines.length > 0 && codeLines[codeLines.length - 1] == '' && commentLines[commentLines.length - 1] == '')
      {
        codeLines.pop();
        commentLines.pop();
      }
      const $code = document.createElement('div');
      $code.classList.add('code');
      this.appendChild($code);
      if (this.hasAttribute('width')) {
        $code.style.width = `${this.getAttribute('width')}px`;
      }
      const editor = monaco.editor.create($code, {
        value: codeLines.join('\n'),
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
        lineNumbers: originalLineNumber => lineMap[originalLineNumber]
      });
      const declarations = [];
      for (let i = 0; i < commentLines.length; ++i) {
        const comment = commentLines[i];
        const [colorClass, lineColorClass, color] = getColor();
        const $comment = document.createElement('div');
        $comment.textContent = comment ? comment : '';
        $comment.style.color = color;
        this.shadowRoot.querySelector('.comments').appendChild($comment);
        if (commentLines[i] != '') {
          declarations.push({
            range: new monaco.Range(i + 1,1,i + 1,1000), options:
            {
              isWholeLine: true,
              //linesDecorationsClassName: colorClass,
              className: lineColorClass,
              hoverMessage: commentLines[i]
            }
          });
        }
      }
      editor.deltaDecorations([], declarations);
      const updateLines = () => {
        const el = editor.getDomNode();
        const viewOverlays = el.querySelectorAll('.view-overlays > div');
        el.querySelectorAll('.view-line').forEach((node, index) => {
          viewOverlays[index].style.setProperty('--gradient-distance', `${(node.querySelector('span').offsetWidth / (this.hasAttribute('width') ? this.getAttribute('width') : 800) * 100).toFixed(2)}%`);
        });
      };
      editor.onDidChangeCursorPosition((e) => {
        setTimeout(updateLines, 0);
        setTimeout(updateLines, 100);
      });
      editor.onDidChangeCursorSelection(() => {
        //console.log('here');
        setTimeout(updateLines, 0);
        setTimeout(updateLines, 100);
      });
      
      setTimeout(() => {
        const el = editor.getDomNode();
        el.style.height = 0;
        editor.layout();
        let height = editor.getScrollHeight();
        //console.log(height);
        el.style.width = el.parentNode.style.width;
        el.style.height = `${height + 7}px`;
        editor.layout();
        setTimeout(() => {
          updateLines();
        }, 0);
      }, 0);
      })();
    });
  }
});