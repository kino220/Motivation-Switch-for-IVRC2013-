#pragma strict
var firstpsx = 50;
var firstpsy = 17;
var firstpsz = 0;

var psx;
var psy;
var psz;

var spd = 0.1;
function Start () {
	
}

function Update () {
	transform.position -= Vector3(spd,0,0);
	
	if(transform.position.x < 0 ){
		transform.position = Vector3(50,17,0);
	}
}