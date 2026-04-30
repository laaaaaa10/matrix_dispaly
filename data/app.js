function sendText() {
  const text = document.getElementById("text").value;
  fetch("/send?text=" + encodeURIComponent(text));
}