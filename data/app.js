// Keep track of the current scroll direction: 1 is forward, -1 is backward.
let currentDirection = 1;
let labelTimer = null;

// Sets the center label and resets it to "made by Javier" after 3 seconds.
function setLabel(html, color) {
  const centerLabel = document.querySelector(".center-label");
  centerLabel.innerHTML = html;
  centerLabel.style.color = color;
  clearTimeout(labelTimer);
  labelTimer = setTimeout(() => {
    centerLabel.innerHTML = "made by Javier";
    centerLabel.style.color = "";
  }, 3000);
}

// This function runs when the user clicks the send button.
function sendText() {
  // Read the message text, speed, and direction from the page.
  const text = document.getElementById("text").value;
  const speed = document.getElementById("speed").value;

  // If there is no text, tell the user to type something first.
  if (!text) {
    setLabel("Please type a message.", "#ef4444");
    return;
  }

  // Send the text, speed, and direction to the ESP web server.
  fetch(
    "/send?text=" + encodeURIComponent(text) +
      "&speed=" + encodeURIComponent(speed) +
      "&direction=" + encodeURIComponent(currentDirection)
  )
    .then(() => setLabel("Message sent<br>successfully!", "#10b981"))
    .catch(() => setLabel("Failed to send<br>message.", "#ef4444"));
}

// Toggle the scroll direction when the user clicks the direction button.
function toggleDirection() {
  currentDirection = currentDirection === 1 ? -1 : 1;
  document.getElementById("directionButton").textContent =
    currentDirection === 1 ? "direction: →" : "direction: ←";
}

// Adjust the textarea height to fit its content.
function resizeTextarea(textarea) {
  textarea.style.height = "auto";
  textarea.style.height = textarea.scrollHeight + "px";
}

// Asks the ESP for its current time and updates the paragraph in the info card.
function updateEspTime() {
  fetch("/time")
    .then(r => r.text())
    .then(t  => { document.getElementById("estTime").textContent = "EST time: " + t; })
    .catch(()  => { document.getElementById("estTime").textContent = "EST time: unavailable"; });
}

// Wire up the page after it loads.
document.addEventListener("DOMContentLoaded", () => {
  const speedInput = document.getElementById("speed");
  const speedValue = document.getElementById("speedValue");
  const textArea = document.getElementById("text");

  // Show the current slider value when the page first opens.
  speedValue.textContent = speedInput.value;

  // Update the percent text as the user moves the slider.
  speedInput.addEventListener("input", () => {
    speedValue.textContent = speedInput.value;
  });

  // Auto-grow the textarea as the user types.
  textArea.addEventListener("input", () => resizeTextarea(textArea));
  resizeTextarea(textArea);

  // Send the browser's current time to the ESP, then start polling once
  // the sync has actually been received. This prevents /time from being
  // called before the ESP knows what time it is.
  fetch("/sync?ts=" + Math.floor(Date.now() / 1000))
    .then(() => {
      updateEspTime();
      setInterval(updateEspTime, 1000);
    })
    .catch(() => {
      document.getElementById("estTime").textContent = "EST time: unavailable";
    });
});