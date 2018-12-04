var address = window.location.hostname;
var urlBase = "";

var postColorTimer = {};
var postValueTimer = {};

var ignoreColorChange = false;

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

	$.get(urlBase + "calibration", function(data) {
		$("#status").html("Loading, please wait...");
		
		$.each(data, function(index, field) {
			if (field.type == "Number") {
				addNumberField(field);
			} else if (field.type == "Boolean") {
				addBooleanField(field);
			} else if (field.type == "Section") {
				addSectionField(field);
			}
		});
		
		$("#status").html("Ready");
    })
    .fail(function(errorThrown) {
		console.log("error: " + errorThrown);
    });
});

function addNumberField(field) {
  var template = $("#numberTemplate").clone();

  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var label = template.find(".control-label");
  label.attr("for", "input-" + field.name);
  label.text(field.label);

  var input = template.find(".input");
  var slider = template.find(".slider");
  slider.attr("id", "input-" + field.name);
  if (field.min) {
    input.attr("min", field.min);
    slider.attr("min", field.min);
  }
  if (field.max) {
    input.attr("max", field.max);
    slider.attr("max", field.max);
  }
  if (field.step) {
    input.attr("step", field.step);
    slider.attr("step", field.step);
  }
  input.val(field.value);
  slider.val(field.value);

  slider.on("change mousemove", function() {
    input.val($(this).val());
  });

  slider.on("change", function() {
    var value = $(this).val();
    input.val(value);
    field.value = value;
    delayPostValue(field.name, value);
  });

  input.on("change", function() {
    var value = $(this).val();
    slider.val(value);
    field.value = value;
    delayPostValue(field.name, value);
  });

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
  var template = $("#sectionTemplate").clone();

  template.attr("id", "form-group-section-" + field.name);
  template.attr("data-field-type", field.type);

  //var label = template.find(".control-label");
  //label.text(field.label);
  
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

function delayPostValue(name, value) {
  clearTimeout(postValueTimer);
  postValueTimer = setTimeout(function() {
    postValue(name, value);
  }, 300);
}