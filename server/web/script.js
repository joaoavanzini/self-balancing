let scene, camera, rendered, cube;

function parentWidth(elem) {
  return elem.parentElement.clientWidth;
}

function parentHeight(elem) {
  return elem.parentElement.clientHeight;
}

function init3D(){
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xffffff);

  camera = new THREE.PerspectiveCamera(75, parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube")), 0.1, 1000);

  renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

  document.getElementById('3Dcube').appendChild(renderer.domElement);

  // Create a geometry
  const geometry = new THREE.BoxGeometry(5, 1, 4);

  // Materials of each face
  var cubeMaterials = [
    new THREE.MeshBasicMaterial({color:0x03045e}),
    new THREE.MeshBasicMaterial({color:0x023e8a}),
    new THREE.MeshBasicMaterial({color:0x0077b6}),
    new THREE.MeshBasicMaterial({color:0x03045e}),
    new THREE.MeshBasicMaterial({color:0x023e8a}),
    new THREE.MeshBasicMaterial({color:0x0077b6}),
  ];

  const material = new THREE.MeshFaceMaterial(cubeMaterials);

  cube = new THREE.Mesh(geometry, material);
  scene.add(cube);
  camera.position.z = 5;
  renderer.render(scene, camera);
}

// Resize the 3D object when the browser window changes size
function onWindowResize(){
  camera.aspect = parentWidth(document.getElementById("3Dcube")) / parentHeight(document.getElementById("3Dcube"));
  //camera.aspect = window.innerWidth /  window.innerHeight;
  camera.updateProjectionMatrix();
  //renderer.setSize(window.innerWidth, window.innerHeight);
  renderer.setSize(parentWidth(document.getElementById("3Dcube")), parentHeight(document.getElementById("3Dcube")));

}

window.addEventListener('resize', onWindowResize, false);

function onError(message) {
  // Ocorrencia de erro
  console.log("Falha: " + message.errorCode + " " + message.errorMessage);
  setTimeout(MQTTConnect, reconnectTimeout);
}
function onConnect() {
  // Conexao ao Broker
  console.log("Conectado");
  // mqtt.subscribe("selfbalancing");
  mqtt.subscribe("selfbalancing");
}

function getTime(){
  return Math.floor(new Date().getTime() / 1000);
}

var mqtt;
var reconnectTimeout = 2000;
var host = "127.0.0.1";
var port = 9001;

function MQTTConnect() {
  // Conecta no Broker
  console.log("Conectando " + host + " " + port);
  mqtt = new Paho.MQTT.Client(host, port, "abcde" + parseInt(Math.random() * 1e5));
  var options = { timeout: 10,
                  keepAliveInterval: 10,
                  onSuccess: onConnect,
                  onFailure: onError
                };
  
  mqtt.connect(options);

  mqtt.onMessageArrived = onMessageArrived;
  mqtt.onConnectionLost = onError;

}

init3D();

function onMessageArrived(msg){
  
  time = getTime();

  var currentTime = Math.floor(new Date().getTime() / 100);
  if (!(currentTime > time)){
    return;
  }


  var obj = JSON.parse(msg.payloadString);
  console.log(obj);
  document.getElementById("gyroX").innerHTML = obj.measure.gyrox;
  document.getElementById("gyroY").innerHTML = obj.measure.gyroy;
  document.getElementById("gyroZ").innerHTML = obj.measure.gyroz;
  document.getElementById("accX").innerHTML = obj.measure.accx;
  document.getElementById("accY").innerHTML = obj.measure.accy;
  document.getElementById("accZ").innerHTML = obj.measure.accz;

  //Change cube rotation after receiving the readinds
  cube.rotation.x = obj.measure.gyrox;
  cube.rotation.z = obj.measure.gyroy;
  cube.rotation.y = obj.measure.gyroz;

  renderer.render(scene, camera);

  console.log(obj.measure.gyrox);

}
  
function resetPosition(element){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/"+element.id, true);
  console.log(element.id);
  xhr.send();
}
