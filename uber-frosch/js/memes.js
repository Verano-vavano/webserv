const upload = document.getElementById("uploadButton");

function add_image() {
	console.log("Bonjour :)");
	var fileInput = document.getElementById('fileInput');
	var file = fileInput.files[0];
	var formData = new FormData();
	formData.append('image', file);

	fetch('/upload/newtmp', {
		method: 'POST',
		body: formData
	})
}

upload.addEventListener("click", add_image);
