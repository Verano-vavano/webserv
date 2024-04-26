const form = document.getElementById("form")
const connect = document.getElementById("connect")
const connect_secure = document.getElementById("connectsec")
const disconnect = document.getElementById("disconnect")

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
		setup_form();
	} else if (res_json.status == "already"){
		display_popup("Tu es deja authentifie");
		console.log(`tu es deja authentifie!`);
	} else {
		display_popup("C'est le mauvais mot de passe");
	}
}

async function connect_user(event){
	event.preventDefault();
	const user = document.getElementById("username").value;
	const password = document.getElementById("password").value;
	// // window.location.href = "http://localhost:8080/"
	const res = await fetch(`/client`, {
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

async function connect_user_cgi(event){
	event.preventDefault();
	const user = document.getElementById("username").value;
	const password = document.getElementById("password").value;
	// // window.location.href = "http://localhost:8080/"
	const res = await fetch(`/client-secure`, {
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


function getCookie(cname) {
  let name = cname + "=";
  let decodedCookie = decodeURIComponent(document.cookie);
  let ca = decodedCookie.split(';');
  for(let i = 0; i <ca.length; i++) {
    let c = ca[i];
    while (c.charAt(0) == ' ') {
      c = c.substring(1);
    }
    if (c.indexOf(name) == 0) {
      return c.substring(name.length, c.length);
    }
  }
  return "";
}

async function disconnect_user(event) {
	event.preventDefault();
	auth = getCookie("session_id")
	await fetch(`/client`, {
		method: "DELETE",
		body: `${auth}`,
		headers: {
			"Content-type": "application/json; charset=UTF-8"
		}
	});
	setup_form();
}

connect.addEventListener("click", connect_user)
connect_secure.addEventListener("click", connect_user_cgi)
disconnect.addEventListener("submit", disconnect_user)

function setup_form() {
	var auth=getCookie("session_id");
	console.log(`auth is ${auth}`);
	if (auth != "") {
		disconnect.style.display = "block";
		form.style.display = "none";
	} else {
		form.style.display = "block"
		disconnect.style.display = "none";
	}
}

window.onload = setup_form
