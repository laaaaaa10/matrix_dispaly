// Keep track of the current scroll direction: 1 is forward, -1 is backward.
let currentDirection = 1;

// This function runs when the user clicks the send button.
function sendText() {
  // Read the message text, speed, and direction from the page.
  const text = document.getElementById("text").value;
  const speed = document.getElementById("speed").value;

  // If there is no text, tell the user to type something first.
  if (!text) {
    alert("Please type a message before sending.");
    return;
  }

  // Send the text, speed, and direction to the ESP web server.
  fetch(
    "/send?text=" + encodeURIComponent(text) +
      "&speed=" + encodeURIComponent(speed) +
      "&direction=" + encodeURIComponent(currentDirection)
  )
    .then(() => {
      const centerLabel = document.querySelector(".center-label");
      centerLabel.innerHTML = "Message sent<br>successfully!";
      centerLabel.style.color = "#10b981";
      setTimeout(() => {
        centerLabel.innerHTML = "made by Javier";
        centerLabel.style.color = "";
      }, 3000); // Reset after 3 seconds
    })
    .catch(() => {
      const centerLabel = document.querySelector(".center-label");
      centerLabel.innerHTML = "Failed to send<br>message.";
      centerLabel.style.color = "#ef4444";
      setTimeout(() => {
        centerLabel.innerHTML = "made by Javier";
        centerLabel.style.color = "";
      }, 3000);
    });
}

// Toggle the scroll direction when the user clicks the direction button.
function toggleDirection() {
  const button = document.getElementById("directionButton");
  currentDirection = currentDirection === 1 ? -1 : 1;
  button.textContent = currentDirection === 1 ? "direction: →" : "direction: ←";
}

// Adjust the textarea height to fit its content.
function resizeTextarea(textarea) {
  textarea.style.height = "auto";
  textarea.style.height = textarea.scrollHeight + "px";
}

// Wire up the page after it loads.
document.addEventListener("DOMContentLoaded", () => {
  const speedInput = document.getElementById("speed");
  const speedValue = document.getElementById("speedValue");
  const textArea = document.getElementById("text");
  const directionButton = document.getElementById("directionButton");

  // Show the current slider value when the page first opens.
  speedValue.textContent = speedInput.value;

  // Set the initial direction button label.
  directionButton.textContent = "direction: →";

  // Update the percent text as the user moves the slider.
  speedInput.addEventListener("input", () => {
    speedValue.textContent = speedInput.value;
  });

  // Auto-grow the textarea as the user types.
  textArea.addEventListener("input", () => resizeTextarea(textArea));
  resizeTextarea(textArea);
});