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
