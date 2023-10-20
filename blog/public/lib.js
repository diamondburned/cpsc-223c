export function useTimestamp(timeElem, date) {
  timeElem.setAttribute("datetime", date.toISOString());
  timeElem.textContent = date.toLocaleString("en-US", {
    month: "long",
    day: "numeric",
    year: "numeric",
    hour: "numeric",
    minute: "numeric",
  });
}

export function $(target, selector) {
  return target.querySelector(selector);
}

export function escapeAttr(str) {
  return str.replace(/"/g, "&quot;");
}

export function escapeHTML(str) {
  return str.replace(/&/g, "&amp;").replace(/</g, "&lt;");
}

const linkRe = /\b(?:https?:\/\/)?(?:(?:[A-Za-z]+\.)+)[^\s,]+\b/g;
const hrRe = /^-{3,}$/gm;

export function markupify(text) {
  text = escapeHTML(text);
  text = text.replace(hrRe, "<hr>");
  text = text.replace(linkRe, (url) => {
    return `<a href="${escapeAttr(url)}" target="_blank">${url}</a>`;
  });
  return text;
}
