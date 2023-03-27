// background.js

// Listen for the onMessage event
chrome.runtime.onMessage.addListener(function (request, sender, sendResponse) {
    // Check if the message is for compressing data
    if (request.action === 'compress') {
      // Compress the data using a compression algorithm of your choice
      const compressedData = compressData(request.data);
  
      // Send the compressed data back to the content script
      sendResponse({ data: compressedData });
    }
  });
  
  // Function to compress data
  function compressData(data) {
    // Implement your data compression algorithm here
    // ...
  
    // Return the compressed data
    return compressedData;
  }
  