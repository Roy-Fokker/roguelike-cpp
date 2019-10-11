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
const colors = CreatePastels(15, 'dark', 0.5);
const fontColors = CreatePastels(15);

let styles = '';
colors.forEach((color, index) => {
  styles += `.color${index} { background-color: ${color}; }`;
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
        display: flex;
        flex-flow: row nowrap;
      }
      ::slotted(.code) {
        flex: 0 0 auto;
        width: 800px;
        border: 1px solid gray;
      }
      :host > .comments {
        flex: 0 0 auto;
        display: flex;
        flex-flow: column nowrap;
        border: 1px solid gray;
        background-color: #1e1e1e;
        color: white;
      }
      :host > .comments > div {
        font-family: Consolas,"Courier New", monospace;
        font-weight: normal;
        font-size: 14px;
        line-height: 19px;
        letter-spacing: 0px;
        white-space: pre;
        padding-left: 5px;
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
      return [`color${index}`, fontColors[index]];
    }

    require(['vs/editor/editor.main'], () => {
      (async () => {
      const url = this.getAttribute('url');
      // Find #L1-L2
      const lineMatches = url.match(/^.*?#L([0-9]*)(?:-L([0-9]*))?$/);
      let start = null;
      let end = null;
      if (lineMatches != null && lineMatches.length >= 2) {
        start = parseInt(lineMatches[1], 10);
      }
      if (lineMatches != null && lineMatches.length == 3) {
        end = parseInt(lineMatches[2], 10) + 1;
      }
      let code = await (await fetch(url)).text();
      code = code.replace(/\/\*\s*?SPDX-License-Identifier: MIT\s*?\*\/\n\n?/i, '');
      let codeLines = code.split(/\n/g).map(line => {
        const matches = line.match(/^(.*?)(?:\s*\/\/.*)?$/);
        return matches != null && matches.length != 1 ? matches[1] : '';
      });
      //console.log(codeLines);
      let commentLines = code.split(/\n/g).map(line => {
        const matches = line.match(/^.*?\/\/\s?(.*)$/);
        return matches != null && matches.length != 1 ? matches[1] : '';
      });
      if (start) {
        codeLines = codeLines.slice(start, end ? end : start + 1);
        commentLines = commentLines.slice(start, end ? end : start + 1);
      }
      //console.log(commentLines);
      for (let i = 0; i < codeLines.length - 1; ++i) {
        if (codeLines[i] == '' && commentLines[i] != '' && commentLines[i + 1] == '') {
          commentLines[i + 1] = commentLines[i];
          commentLines[i] = '';
        }
        if (codeLines[i] == '' && commentLines[i] == '' && codeLines[i + 1] == '') {
          codeLines.splice(i, 1);
          commentLines.splice(i, 1);
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
        theme: 'vs-dark'
      });
      const declarations = [];
      for (let i = 0; i < commentLines.length; ++i) {
        const comment = commentLines[i];
        const [colorClass, color] = getColor();
        const $comment = document.createElement('div');
        $comment.textContent = comment ? comment : '';
        $comment.style.color = color;
        this.shadowRoot.querySelector('.comments').appendChild($comment);
        if (commentLines[i] != '') {
          declarations.push({
            range: new monaco.Range(i + 1,1,i + 1,1000), options:
            {
              isWholeLine: true,
              linesDecorationsClassName: colorClass,
              hoverMessage: commentLines[i]
            }
          });
        }
      }
      editor.deltaDecorations([], declarations);

      setTimeout(() => {
        const el = editor.getDomNode();
        el.style.height = 0;
        editor.layout();
        let height = editor.getScrollHeight();
        //console.log(height);
        el.style.width = el.parentNode.style.width;
        el.style.height = `${height + 7}px`;
        editor.layout();
      }, 0);
      })();
    });
  }
});