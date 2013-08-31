#pragma strict

var spd:float = 0.01;
var rollspeed = 6;

function Start () {

}

function FixedUpdate() {

	if(Input.GetKey("right")){
		transform.position -= Vector3(100*spd,0,0);
	}
	else if (Input.GetKey("left")) {
		transform.position += Vector3(100*spd,0,0);
	}
	
	
	if (Input.GetKey("up")) {
		transform.position += Vector3(0,100*spd,0);
	}
	else if (Input.GetKey("down")) {
		transform.position -= Vector3(0,100*spd,0);
	}
	
	if (Input.GetKey("w")) {
		transform.position -= Vector3(0,0,100*spd);
	}
	else if (Input.GetKey("s")) {
		transform.position += Vector3(0,0,100*spd);
	}
	
}