var address = window.location.hostname;
var urlBase = "";

var ws = new ReconnectingWebSocket('ws://' + address + ':81/', ['arduino']);
ws.debug = true;

ws.onopen = function () {
  ws.send('Connect ' + new Date());
};

ws.onmessage = function(evt) {
		console.log('Server: ', evt.data);
  if(evt.data != null)
  {
    var data = JSON.parse(evt.data);
    if (data == null) {	
		console.log('Server: ', evt.data);
		return;
	}
    updateFieldValue(data.name, data.value);
  }
}

ws.onerror = function (error) {
  console.log('WebSocket Error ', error);
};

ws.onclose = function () {
  console.log('WebSocket connection closed');
};


$(document).ready(function() {
	$("#status").html("Connecting, please wait...");

	$.get(urlBase + "stats", function(data) {
		$("#status").html("Loading stats, please wait...");
			//console.log("error: ..have we got any stats data?");
		$.each(data, function(index, field) {
			//console.log("error: each stats data");
			if (field.type == "Stat") {
				addStatField(field);
			} else if (field.type == "Boolean") {
				addBooleanField(field);
			} else if (field.type == "Section") {
				addSectionField(field);
			} else if (field.type == "Button") {
				addButtonField(field);
			}
		});
		$("#status").html("Ready");
	})
	.fail(function(errorThrown) {
	  console.log("error: " + errorThrown);
	});
});

function addStatField(field) {
  console.log("function: addStatField");
  var template = $("#statTemplate").clone();

  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var id = "input-" + field.name;

  var label = template.find(".control-label");
  label.attr("for", id);
  //hijacking the control label using for name and value
  var tx = field.label + " - " + field.text;
  label.text(tx);
  
  $("#form").append(template);
}

function addBooleanField(field) {
  var template = $("#booleanTemplate").clone();

  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var label = template.find(".control-label");
  label.attr("for", "btn-group-" + field.name);
  label.text(field.label);

  var btngroup = template.find(".btn-group");
  btngroup.attr("id", "btn-group-" + field.name);

  var btnOn = template.find("#btnOn");
  var btnOff = template.find("#btnOff");

  btnOn.attr("id", "btnOn" + field.name);
  btnOff.attr("id", "btnOff" + field.name);

  btnOn.attr("class", field.value ? "btn btn-primary" : "btn btn-default");
  btnOff.attr("class", !field.value ? "btn btn-primary" : "btn btn-default");

  btnOn.click(function() {
    setBooleanFieldValue(field, btnOn, btnOff, 1)
  });
  btnOff.click(function() {
    setBooleanFieldValue(field, btnOn, btnOff, 0)
  });

  $("#form").append(template);
}

function addSectionField(field) {
  console.log("function: addSectionField");
  var template = $("#sectionTemplate").clone();

  template.attr("id", "form-group-section-" + field.name);
  template.attr("data-field-type", field.type);

  var label = template.find(".control-label");
  label.text(field.label);
  
  $("#form").append(template);
}

function addButtonField(field) {
  var template = $("#buttonTemplate").clone();

  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var label = template.find(".control-label");
  label.attr("for", "btn-" + field.name);
  label.text(field.label);

  //var btngroup = template.find(".btn-group");
  //btngroup.attr("id", "btn-group-" + field.name);

  var btnOff = template.find("#btnOff");

  btnOff.attr("id", "btnOff" + field.name);

  btnOff.attr("class", field.value ? "btn btn-primary" : "btn btn-default");	//???

  btnOff.click(function() {
    //setBooleanFieldValue(field, btnOn, btnOff, 1)
	//field.value = ;
	postValue(field.name, field.value);
  });

  $("#form").append(template);
}

function updateFieldValue(name, value) {
  $("#status").html(name + ": " + value + ", updated.");
  var group = $("#form-group-" + name);

  var type = group.attr("data-field-type");

  if (type == "Number") {
    var input = group.find(".form-control");
    input.val(value);
  } else if (type == "Boolean") {
    var btnOn = group.find("#btnOn" + name);
    var btnOff = group.find("#btnOff" + name);

    btnOn.attr("class", value ? "btn btn-primary" : "btn btn-default");
    btnOff.attr("class", !value ? "btn btn-primary" : "btn btn-default");

  } else if (type == "Select") {
    var select = group.find(".form-control");
    select.val(value);
  } else if (type == "Color") {
    var input = group.find(".form-control");
    input.val("rgb(" + value + ")");
  } else if (type == "Button") {
	//..don't need to update, that's the whole point
    //var btn = group.find("#btn" + name);
    //btn.attr("class", value ? "btn btn-primary" : "btn btn-default");
  }
};

function setBooleanFieldValue(field, btnOn, btnOff, value) {
  field.value = value;

  btnOn.attr("class", field.value ? "btn btn-primary" : "btn btn-default");
  btnOff.attr("class", !field.value ? "btn btn-primary" : "btn btn-default");

  postValue(field.name, field.value);
}

function postValue(name, value) {
	$("#status").html("Setting " + name + ": " + value + ", please wait...");

	var body = { name: name, value: value };

	var myJSON = JSON.stringify(body);
	ws.send(myJSON);
	
	$("#status").html("Set " + name + ": " + value);
}
