#pragma strict

var spd:float = 1;
var rollspeed = 6;

var target:Transform;

function FixedUpdate() {

	if(Input.GetKey("up")){
		transform.eulerAngles -= Vector3(100*spd,0,0);
	}
	else if (Input.GetKey("down")) {
		transform.eulerAngles += Vector3(100*spd,0,0);
	}
	
	
	if (Input.GetKey("right")) {
		transform.eulerAngles -= Vector3(0,100*spd,0);
	}
	else if (Input.GetKey("left")) {
		transform.eulerAngles += Vector3(0,100*spd,0);
	}
	
}

function Update () {
//transform.Rotate(10, 50 * Time.deltaTime, 10);
target = GameObject.Find("CameraA").transform;

transform.LookAt(target);


}