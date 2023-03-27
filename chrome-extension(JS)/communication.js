// content.js

// Send a message to the background script to compress the data
chrome.runtime.sendMessage({ action: 'compress', data: myData }, function (response) {
  // Handle the response from the background script
  if (response) {
    // The data has been compressed
    console.log('Compressed data:', response.data);
  } else {
    // An error occurred
    console.error('Error compressing data');
  }
});
