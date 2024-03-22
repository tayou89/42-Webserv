function deleteFile(fileName) 
{
  if (fileName) {
    var url = `/rm/${encodeURIComponent(fileName)}`; 
    fetch(url, { method: 'DELETE', headers: {'Content-Type': 'application/json',},})
  .then(response => {
    if (!response.ok) {
      throw new Error(`HTTP error! Status: ${response.status}`);
    }
    return response.json(); // Process the response body if needed
  })
  .then(data => {
    console.log('DELETE request successful:', data);
    // Handle the response data or perform additional actions as needed
  })
  .catch(error => {
    console.error('Error:', error);
    // Handle errors or display an error message to the user
  });
  } else {
    alert("No file selected");
  }
	location.href="/delete/deletePage.py";
}
