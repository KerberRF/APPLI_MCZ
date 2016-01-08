var util = require('util')
var exec = require('child_process').exec;
var sleep = require('sleep');

var data = [
    {
      "id": "0", "url": "/switches/0", "name": "Power", "script": "sudo /home/pi/Dev/APPLI_MCZ/src/Appli_Cmd_Arg", "Id": "9859842 ", "Mode": "0", "User": "2", "Puissance" :"0", "Ventilateur1" : "0", "Ventilateur2" : "0"
    },
 ]; 
// GET
exports.switches = function (req, res) {
  console.log('Getting switches.');
  var switches = [];
  res.json(data);
};
 
// PUT
exports.editSwitch = function (req, res) {
  var id = req.params.id;
  if (id >= 0 && id <= data.length) {
    console.log('Switch Status of switch with id: ' + id + " to " + req.body.status);
    var script = data[id].script;
    var command = data[id].command;
    switchStatus(script,command,req.body.status);
    data[id].status = req.body.status;
    res.send(200);
  } else {
    res.json(404);
  }
};
 
function switchStatus(script, command, status){
    var execString = script + " " + command + " " + status;
    console.log("Executing: " + execString);
    exec(execString, puts);
}
