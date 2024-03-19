const form = document.getElementById("form")

function remove_popup() {
	var popup = document.getElementsByClassName("popup");
	for (var i = 0; i < popup.length; i++) {
		document.body.removeChild(popup[i]);
	}
}

function display_popup(message) {
	remove_popup();
	const div = document.createElement("div");
	div.innerHTML = `<h1>${message}</h1>`
	div.classList.add("popup");
	document.body.appendChild(div);
	setTimeout(remove_popup, 3000)
}

function handle_res(res_json) {
	if (res_json.status == "created") {
		display_popup("C'est bon tu viens de creer un utilisateur!")
	} else if (res_json.status == "ok"){
		display_popup("Tu es authentifie!");
		console.log(`le cookie d'auth est ${res_json.auth}`)
		document.cookie = `auth=${res_json.auth}`
	} else if (res_json.status == "already"){
		display_popup("Tu es deja authentifie");
		console.log(`tu es deja authentifie!`);
	} else {
		display_popup("C'est le mauvais mot de passe");
	}
}

async function handle_post(event){
	event.preventDefault();
	const user = document.getElementById("username").value;
	const password = document.getElementById("password").value;
	// // window.location.href = "http://localhost:8080/"
	const res = await fetch(`${window.location}`, {
		method: "POST",
		body: JSON.stringify({
			user: `${user}`,
			password: `${password}`,
		}),
		headers: {
			"Content-type": "application/json; charset=UTF-8"
		}
	});
	handle_res(await res.json());
}


form.addEventListener("submit", handle_post)
