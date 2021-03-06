SELAIN_JS_STRINGIFY((() => {
  const original = window.SelainHintMode;
  const queryExpression = '//*[@onclick or @onmouseover or @onmousedown or ' +
    '@onmouseup or @oncommand or @href] | //input[not(@type="hidden")] | ' +
    '//a[href] | //area | //textarea | //button | //select';
  const maxAllowedHints = 500;
  const hintChars = Array.from('sadfjklewc'.toUpperCase());
  const hints = [];
  let hintContainer = null;
  let currentSequence = '';
  let openToNewTab = false;

  const numberToSequence = (number) => Array
    .from(`${number}`)
    .map((ch) => hintChars[parseInt(ch, 10)]).join('');

  const install = () => {
    const topWindow = window;
    const topWindowHeight = topWindow.innerHeight;
    const topWindowWidth = topWindow.innerWidth;
    let hintCount = 0;

    const drawHintsToWindow = (win, offsetX, offsetY) => {
      const doc = win.document;
      const winHeight = win.height;
      const winWidth = win.width;

      // Bounds
      const minX = offsetX < 0 ? -offsetX : 0;
      const minY = offsetY < 0 ? -offsetY : 0;
      const maxX = (
        offsetX + winWidth > topWindowWidth
          ? topWindowWidth - offsetX
          : topWindowWidth
      );
      const maxY = (
        offsetY + winHeight > topWindowHeight
          ? topWindowHeight - offsetY
          : topWindowHeight
      );

      const { scrollX, scrollY } = win;

      hintContainer = doc.createElement('div');

      const result = doc.evaluate(
        queryExpression,
        doc,
        () => 'http://www.w3.org/1999/xhtml',
        XPathResult.ORDERED_NODE_SNAPSHOT_TYPE,
        null
      );

      const hintSpan = doc.createElement('span');
      hintSpan.style.zIndex = '100000';
      hintSpan.style.position = 'absolute';
      hintSpan.style.padding = '0.2em';
      hintSpan.style.borderColor = 'rgba(0, 0, 0, 0.4)';
      hintSpan.style.borderWidth = '0.15em';
      hintSpan.style.border = '';
      hintSpan.style.backgroundColor = '#ffd76e';
      hintSpan.style.color = '#000000';
      hintSpan.style.fontFamily = 'monospace';
      hintSpan.style.fontSize = '1em';
      hintSpan.style.lineHeight = '1';
      hintSpan.style.fontWeight = 'bold';
      hintSpan.style.whiteSpace = 'nowrap';
      hintSpan.style.textShadow = 'none';

      for (let i = 0; i < result.snapshotLength; ++i) {
        if (hintCount >= maxAllowedHints) {
          break;
        }

        const element = result.snapshotItem(i);
        const rectangle = element.getBoundingClientRect();

        if (!rectangle ||
            rectangle.left > maxX ||
            rectangle.right < minX ||
            rectangle.top > maxY ||
            rectangle.bottom < minY) {
          continue;
        }

        const style = topWindow.getComputedStyle(element, '');
        if (style.display === 'none' || style.visibility !== 'visible') {
          continue;
        }

        const leftPos = Math.max(rectangle.left + scrollX, scrollX);
        const topPos = Math.max(rectangle.top + scrollY, scrollY);

        const sequence = numberToSequence(++hintCount);

        const hint = hintSpan.cloneNode(false);
        hint.style.left = `${leftPos}px`;
        hint.style.top = `${topPos}px`;
        hint.innerText = sequence;
        hintContainer.appendChild(hint);

        hints.push({ element, sequence, span: hint });
      }

      doc.documentElement.appendChild(hintContainer);

      // Also go through the frames contained in the document.
      ['frame', 'iframe'].forEach((tagName) => {
        const frames = doc.getElementsByTagName(tagName);

        for (let i = 0; i < frames.length; ++i) {
          const element = frames[i];
          const rectangle = element.getBoundingClientRect();

          if (!element.contentWindow ||
              !rectangle ||
              rectangle.left > maxX ||
              rectangle.right < minX ||
              rectangle.top > maxY ||
              rectangle.bottom < minY) {
            continue;
          }
          drawHintsToWindow(
            element.contentWindow,
            offsetX + rectangle.left,
            offsetY + rectangle.top
          );
        }
      });
    };

    drawHintsToWindow(topWindow, 0, 0);
  };

  const uninstall = () => {
    hints
      .filter((hint) => typeof hint.element !== 'undefined')
      .forEach((hint) => {
        hint.span.parentNode.removeChild(hint.span);
      });
    hints.length = 0;
    if (hintContainer) {
      hintContainer.parentNode.removeChild(hintContainer);
      hintContainer = null;
    }
    window.SelainHintMode = original;
  };

  const activateHint = (hint) => {
    if (!hint || typeof hint.element === 'undefined') {
      return 'ignore';
    }

    const { element } = hint;
    const tagName = element.nodeName.toLowerCase();

    uninstall();

    if (['input', 'select', 'textarea'].indexOf(tagName) >= 0 &&
        ['button', 'submit'].indexOf(element.type) < 0) {
      element.focus();

      return 'mode::insert';
    } else if (['frame', 'iframe'].indexOf(tagName) >= 0) {
      element.focus();
    } else if (openToNewTab) {
      const oldTarget = element.getAttribute('target');

      element.setAttribute('target', '_blank');
      element.click();
      element.setAttribute('target', oldTarget);
    } else {
      element.click();
    }

    return 'mode::normal';
  };

  const splitHints = () => {
    const matches = [];
    const nonMatches = [];

    if (currentSequence.length > 0) {
      hints.forEach((hint) => {
        if (hint.sequence.startsWith(currentSequence)) {
          matches.push(hint);
        } else {
          nonMatches.push(hint);
        }
      });
    } else {
      nonMatches.push(...hints);
    }

    return [matches, nonMatches];
  };

  const updateMatches = (matches, nonMatches) => {
    if (currentSequence.length > 0) {
      matches.forEach((hint) => {
        const { sequence, span } = hint;
        const matchingPart = span.ownerDocument.createElement('span');
        const nonMatchingPart = span.ownerDocument.createElement('span');

        if (span.style.visibility !== 'visible') {
          span.style.visibility = 'visible';
        }

        matchingPart.innerText = currentSequence;
        nonMatchingPart.innerText = sequence.substr(currentSequence.length);

        matchingPart.style.color = '#a07555';

        span.innerHTML = '';
        span.appendChild(matchingPart);
        span.appendChild(nonMatchingPart);
      });
      nonMatches.forEach((hint) => {
        const { span } = hint;

        if (span.style.visibility !== 'hidden') {
          span.style.visibility = 'hidden';
        }
      });
    } else {
      [...matches, ...nonMatches].forEach((hint) => {
        const { sequence, span } = hint;

        if (span.style.visibility !== 'visible') {
          span.style.visibility = 'visible';
        }
        span.innerText = sequence;
      });
    }
  };

  const addChar = (ch) => {
    if (typeof ch !== 'string' ||
        ch.length !== 1 ||
        hintChars.indexOf(ch.toUpperCase()) < 0) {
      return 'ignore';
    }

    currentSequence += ch.toUpperCase();

    const [matches, nonMatches] = splitHints();

    if (matches.length === 1) {
      return activateHint(matches[0]);
    }

    updateMatches(matches, nonMatches);

    return 'ignore';
  };

  const removeChar = () => {
    if (currentSequence.length > 0) {
      currentSequence = currentSequence.substr(0, currentSequence.length - 1);
      updateMatches(...splitHints());
    }
  };

  const activateCurrentMatch = () => {
    if (currentSequence.length <= 0) {
      return 'ignore';
    }

    const match = hints.find((hint) => hint.sequence === currentSequence);

    return match ? activateHint(match) : 'ignore';
  };

  const setOpenToNewTab = () => {
    openToNewTab = true;
  };

  install();

  window.SelainHintMode = {
    activateCurrentMatch,
    addChar,
    removeChar,
    setOpenToNewTab,
    uninstall
  };
})();)
