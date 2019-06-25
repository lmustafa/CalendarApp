'use strict'

// C library API
const ffi = require('ffi');
// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

const mysql = require('mysql');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
//******************** Your code goes here ********************

let sharedLib = ffi.Library("./parser/bin/libcal.dylib",{
	"objectToJSON" : ["string", ["string"]],
	"eventsToJSON" : ["string", ["string"]],
	"dataToEvents" : ["string", ["string","string","string","string","string","string","string","string"]],
	"alarmsToJSON" : ["string", ["string","int"]],
	"createFile"   : ["string", ["string","string","string","string","string","string"]],
	"propsToJSON"  : ["string", ["string","int"]],
});

app.get('/object', function(req, res) {
	let files = fs.readdirSync('./uploads/');
	let array = [];

	let arrOfFiles = [];
	//validate filename.
	let j = 0;
	for(var i = 0; i < files.length; i++) {
		var jString = sharedLib.objectToJSON(files[i]);
		if(jString == "{}") {
			//console.log("EVER HERER");
			//
		}
		else{
			arrOfFiles[j] = files[i];
			j++;
		}
	}

	for(var i = 0; i < arrOfFiles.length; i++) {
		var jString = sharedLib.objectToJSON(arrOfFiles[i]);
		array[i] = JSON.parse(jString)
	}
	res.send(array);
});

app.get('/fileCreated', function(req , res){

	let fileName  = req.query.fileName;
	let version   = req.query.version;
	let prodID    = req.query.prodID;
	let evUID     = req.query.evUID;
	let startDate = req.query.startDate;
	let startTime = req.query.startTime;

	var jString = sharedLib.createFile(fileName, version, prodID, evUID, startDate, startTime);
	var array   = JSON.parse(jString);
	//console.log(array);
	res.send(array);

});

app.get('/addFileToPanel', function(req , res){
	let fileSelected = req.query.filename;
	//let files = fs.readdirSync('./uploads/');
	//res.send(fileSelected);
	let f = "./uploads/"
	f = f + fileSelected;
	var jString = sharedLib.objectToJSON(f);
	//var jString = sharedLib.objectToJSON(fileSelected);
	var array   = JSON.parse(jString);
	console.log(f);
	res.send(array);
});

app.get('/tempCal', function(req , res){
	const filename = req.query.filename;
	var jString = sharedLib.eventsToJSON(filename);
	var array   = JSON.parse(jString);
	res.send(array);
});
//
app.get('/addEvent', function(req , res){
	const filename    = req.query.filename;
	const uid         = req.query.uid;
	const summary     = req.query.summary;
	const startDate   = req.query.startDate;
	const startTime   = req.query.startTime;
	const evstartDate = req.query.evstartDate;
	const evstartTime = req.query.evstartTime;
	const utc         = req.query.utc;

	var jString = sharedLib.dataToEvents(filename,summary, uid, startDate, startTime,evstartDate,evstartTime,utc);
	var array   = JSON.parse(jString);
	res.send(array);
});

app.get('/displayAlarm', function(req , res){
	const filename = req.query.filename;
	const num = req.query.num;
	let jString = sharedLib.alarmsToJSON(filename, num);
	let array;
	var myJSON = '{"action":"DNE"}';

	if(jString == "[{}]") {
		//res.send(myObj);
		array = JSON.parse(myJSON);
	}
	else{
		array   = JSON.parse(jString);
	}

	res.send(array);
});

app.get('/displayProp', function(req , res){
	const filename = req.query.filename;
	const num = req.query.num;

	let jString = sharedLib.propsToJSON(filename, num);
	let array;

	var myJSON = '{"propName":"DNE"}';
  //var myObj  = JSON.parse(myJSON);

	if(jString == "[{}]") {
		//res.send(myObj);
		array = JSON.parse(myJSON);
	}
	else{
		array = JSON.parse(jString);
	}

	res.send(array);
});

app.get('/fileList', function(req , res){
		let files = fs.readdirSync('./uploads/');

		let arrOfFiles = [];
		let j = 0;
		for(var i = 0; i < files.length; i++) {
			var jString = sharedLib.objectToJSON(files[i]);
			if(jString == "{}") {
				//
			}
			else{
				arrOfFiles[j] = files[i];
				j++;
			}
		}
    res.send(arrOfFiles);
});

app.get('/invalidfileList', function(req , res){
	let files = fs.readdirSync('./uploads/');

	let arrOfFiles = [];
	let j = 0;
	for(var i = 0; i < files.length; i++) {
		var jString = sharedLib.objectToJSON(files[i]);
		if(jString == "{}") {
			arrOfFiles[j] = files[i];
			j++;
		}
		else{
			//
		}
	}
	res.send(arrOfFiles);
});

/****DATABASE***/

let username = "";
let password = "";

let connection = null;

app.get('/loginDatabase', function(req , res){

	username = req.query.username;
	password = req.query.password;
	//let database = req.query.database;
	let string      = "tables created";
	connection = mysql.createConnection({
 		host : 'dursley.socs.uoguelph.ca',
 		user : username,
 		password : password,
 		database : username
	});

		connection.connect(function(err) {
			if(err) {
				console.log("r we ever herere");
				res.send("DNE");
			}
			else{
				let createFile = "CREATE TABLE IF NOT EXISTS FILE"
					+ " (cal_id INT AUTO_INCREMENT,"
					+ " file_Name VARCHAR(60) NOT NULL,"
					+ " version INT NOT NULL,"
					+ " prod_id VARCHAR(256) NOT NULL,"
					+ " PRIMARY KEY(cal_id));"

					//console.log(createFile);

				connection.query(createFile, function (err, rows, fields) {
			 		//Display an error if we cannot run the query
			 		if (err)
			 			console.log("Something went wrong with creating FILE TABLE. "+err);
			 		else {
						//
			 		}
				});

				let createEvent = "CREATE TABLE IF NOT EXISTS EVENT"
					+ " (event_id INT AUTO_INCREMENT,"
					+ " summary VARCHAR(1024),"
					+ " start_time DATETIME NOT NULL,"
					+ " location VARCHAR(60),"
					+ " organizer VARCHAR(256),"
					+ " cal_file INT NOT NULL,"
					+ " PRIMARY KEY(event_id),"
					+ " FOREIGN KEY(cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE);"

					connection.query(createEvent, function (err, rows, fields) {
						//Display an error if we cannot run the query
						if (err)
							console.log("Something went wrong with creating EVENT TABLE. "+err);
						else {

						}
					});

					let createAlarm = "CREATE TABLE IF NOT EXISTS ALARM"
						+ " (alarm_id INT AUTO_INCREMENT,"
						+ " action VARCHAR(256) NOT NULL,"
						+ " `trigger` VARCHAR(256) NOT NULL,"
						+ " event INT NOT NULL,"
						+ " PRIMARY KEY(alarm_id),"
						+ " FOREIGN KEY(event) REFERENCES EVENT(event_id) ON DELETE CASCADE);"

						connection.query(createAlarm, function (err, rows, fields) {
							//Display an error if we cannot run the query
							if (err)
								console.log("Something went wrong with creating ALARM TABLE. "+err);
							else {

							}
						});
				res.send(string);
			}
		});
});

//let val = 0;

app.get('/checkLength', function(req , res){

	let num = 0;

		connection.query("SELECT * FROM FILE;", function (err, result, fields) {
			//Throw an error if we cannot run the query
			if (err) {
					console.log("Something went wrong with checking the length of the file. "+err);
			}
			else {
				res.send(result);
			}
		});

});

app.get('/StoreAllFiles', function(req , res){


	toClear();

	let files = fs.readdirSync('./uploads/');

	if(files.length == 0) {
		console.log("no files in uploads");
		//res.send("there are no ")
		//return;
	}
	let arrOfFiles = [];
	let j = 0;
	for(let i = 0; i < files.length; i++) {
		var jString = sharedLib.objectToJSON(files[i]);
		if(jString == "{}") {
			//
		}
		else{
			arrOfFiles[j] = files[i];
			j++;
		}
	}
	let numberOfEvents = 0;

	for(let i = 0; i < arrOfFiles.length; i++) {
		let eventVar = i + 1;
		let calString = sharedLib.objectToJSON(arrOfFiles[i]);

		let object = JSON.parse(calString);
		let version = object.version;
		let prodID  = object.prodID;
		let numEvents  = object.numEvents;


		let insertFile = "INSERT INTO FILE (file_Name, version, prod_id) VALUES ("
										 +"\"" + arrOfFiles[i] + "\"" + "," + version + "," + "\"" + prodID + "\"" + ");"

		connection.query(insertFile, function (err, rows, fields) {
	  //Throw an error if we cannot run the query
	    if (err) {
				       console.log("Something went wrong with inserting a file. "+err);
			}
	    else {
				//
	  }});

			for(let k = 0; k < numEvents; k++) {
					numberOfEvents++;
					let eventNum = k + 1;
					let eventString = sharedLib.eventsToJSON(arrOfFiles[i]);
					let eventObject   = JSON.parse(eventString);

					let date1 = eventObject[k].startDT.date;
					let formattedDate = date1.slice(0, 4) + "-" + date1.slice(4, 6) + "-" + date1.slice(6, 8);

					//console.log("startDT = " + formattedDate);
					//let startDT   = eventObject[k].startDT;
					let startDT   = "\"" + formattedDate + "\"";
					let summary   = eventObject[k].summary;
					let numProps  = eventObject[k].numProps;
					let numAlarms = eventObject[k].numAlarms;

					//what does propsToJSON start with 1 or 0?
					let location  = null;
					let organizer = null;
					let summmary  = null;

					let pObject = sharedLib.propsToJSON(arrOfFiles[i],eventNum);

					let propObject = JSON.parse(pObject);

					if(pObject == "[{}]") {
							//
					}
					else{
						for(let v = 0; v < propObject.length; v++) {
							if(propObject[v].propName == "LOCATION") {
									location = propObject[v].propDescr;
							}
							if(propObject[v].propName == "ORGANIZER") {
									organizer = propObject[v].propDescr;
							}
							if(propObject[v].propName == "SUMMARY") {
									summary = propObject[v].propDescr;
							}
						}
					}

				let insertEvent = "";

				if(location == null && organizer == null && summary == null) {
						insertEvent = "INSERT INTO EVENT (summary, start_time,location, organizer, cal_file) VALUES ("
															 + "NULL" + "," + startDT + "," + "NULL" + "," + "NULL" + "," + eventVar + ");"
				}
				else if(location == null && organizer == null) {
					insertEvent = "INSERT INTO EVENT (summary, start_time,location, organizer, cal_file) VALUES ("
												+ "\"" + summary + "\""  + "," + startDT + "," + "NULL" +  "," + "NULL" + "," + eventVar +  ");"
				}
				else if(organizer == null && summary == null) {
					insertEvent = "INSERT INTO EVENT (summary, start_time,location, organizer, cal_file) VALUES ("
											+ "NULL" + "," + startDT + "," + "\"" + location + "\"" + "," + "NULL" + "," + eventVar +  ");"

				}
				else if(location == null && summary == null) {
					insertEvent = "INSERT INTO EVENT (summary, start_time,location, organizer, cal_file) VALUES ("
											+ "NULL" + "," + startDT + "," + "NULL" + "," + "\"" + organizer + "\""  + "," + eventVar +  ");"
				}
				else if(location == null) {
					insertEvent = "INSERT INTO EVENT (summary, start_time,location, organizer, cal_file) VALUES ("
											+ "\"" + summary + "\""  + "," + startDT + ","  + "NULL" +  "," + "\"" + organizer + "\""  + "," + eventVar +  ");"
				}
				else if(summary == null) {
					insertEvent = "INSERT INTO EVENT (summary, start_time,location, organizer, cal_file) VALUES ("
										  + "NULL" + "," + startDT + "," + "\"" + location + "\"" + "," + "\"" + organizer + "\""  + "," + eventVar +  ");"
				}
				else if(organizer == null) {
					insertEvent = "INSERT INTO EVENT (summary, start_time,location, organizer, cal_file) VALUES ("
											+ "\"" + summary + "\""  + "," + startDT + "," + "\"" + location + "\"" + "," + "NULL" + "," + eventVar +  ");"
				}
				else{
						insertEvent = "INSERT INTO EVENT (summary, start_time,location, organizer, cal_file) VALUES ("
												+ "\"" + summary + "\""  + "," + startDT + "," + "\"" + location + "\"" + "," + "\"" + organizer + "\""  + "," + eventVar +  ");"
				}

				connection.query(insertEvent, function (err, rows, fields) {
				//Throw an error if we cannot run the query
					if (err) {
							console.log("Something went wrong with inserting an event. "+err);
					}
					else {

					}});

					let aList = sharedLib.alarmsToJSON(arrOfFiles[i],eventNum);
					let alarmList = JSON.parse(aList);
					if(aList == "[{}]") {
						//
					}
					else{
						for(let n = 0; n < alarmList.length; n++) {
							let action  = alarmList[n].action;
							let trigger = alarmList[n].trigger;

							let insertAlarm = "INSERT INTO ALARM (action, `trigger`, event) VALUES ("
																+ "\"" + action + "\""  + "," + "\"" + trigger + "\""  + "," + numberOfEvents + ");"

							connection.query(insertAlarm, function (err, rows, fields) {
							//Throw an error if we cannot run the query
							if (err) {
									console.log("Something went wrong with inserting an alarm. "+err);
							}
							else {
								//
						 }});
						}
					}
			}

	}
	res.send("string");

});


function toClear() {
	let createFile = "CREATE TABLE IF NOT EXISTS FILE"
		+ " (cal_id INT AUTO_INCREMENT,"
		+ " file_Name VARCHAR(60) NOT NULL,"
		+ " version INT NOT NULL,"
		+ " prod_id VARCHAR(256) NOT NULL,"
		+ " PRIMARY KEY(cal_id));"


	let createEvent = "CREATE TABLE IF NOT EXISTS EVENT"
		+ " (event_id INT AUTO_INCREMENT,"
	  + " summary VARCHAR(1024),"
		+ " start_time DATETIME NOT NULL,"
		+ " location VARCHAR(60),"
		+ " organizer VARCHAR(256),"
		+ " cal_file INT NOT NULL,"
		+ " PRIMARY KEY(event_id),"
		+ " FOREIGN KEY(cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE);"


	 let createAlarm = "CREATE TABLE IF NOT EXISTS ALARM"
		+ " (alarm_id INT AUTO_INCREMENT,"
		+ " action VARCHAR(256) NOT NULL,"
		+ " `trigger` VARCHAR(256) NOT NULL,"
		+ " event INT NOT NULL,"
		+ " PRIMARY KEY(alarm_id),"
		+ " FOREIGN KEY(event) REFERENCES EVENT(event_id) ON DELETE CASCADE);"

		let allTables = createFile + " " + createEvent + " " + createAlarm;

	connection.query("DELETE FROM FILE", function (err, rows, fields) {
  //Throw an error if we cannot run the query
    if (err)
        console.log("Something went wrong with deleting the file. "+err);
    else {
  		//
    }
	});

	connection.query("DROP TABLE ALARM;", function (err, rows, fields) {
		//Throw an error if we cannot run the query
		if (err)
			console.log("Something went wrong with deleting the file. "+err);
			else {
		//
		}
	});

	connection.query("DROP TABLE EVENT;", function (err, rows, fields) {
		//Throw an error if we cannot run the query
		if (err)
			console.log("Something went wrong with deleting the file. "+err);
			else {
		//
		}
	});

	connection.query("DROP TABLE FILE;", function (err, rows, fields) {
		//Throw an error if we cannot run the query
		if (err)
			console.log("Something went wrong with deleting the file. "+err);
			else {
		//
		}
	});

	connection.query(createFile, function (err, rows, fields) {
		//Throw an error if we cannot run the query
		if (err)
			console.log("Something went wrong with deleting the file. "+err);
		else {
				//
			}
		});


	connection.query(createEvent, function (err, rows, fields) {
		//Throw an error if we cannot run the query
		if (err)
			console.log("Something went wrong with deleting the file. "+err);
	  else {
			//
		}
 });

 connection.query(createAlarm, function (err, rows, fields) {
	 //Throw an error if we cannot run the query
	 if (err)
		 console.log("Something went wrong with deleting the file. "+err);
	 else {
		 //
	 }
 });

}

app.get('/clearData', function(req , res){
	toClear();
	res.send("clear data");
});

app.get('/DisplayDB', function(req , res){

	let fileCount  = "SELECT * FROM FILE;"
	let eventCount = "SELECT * FROM EVENT;"
	let alarmCount = "SELECT * FROM ALARM;"

	let fileNum  = 0;
	let eventNum = 0;
	let alarmNum = 0;

	connection.query(fileCount, function (err, rows, fields) {
    if (err)
        console.log("Something went wrong with returning number of files. "+err);
    else {
			//fileNum = rows.length;
			for (let row of rows){
				//console.log(row.version);
				//console.log(fileNum);
				fileNum = fileNum + 1;
			}
    }
});
		connection.query(eventCount, function (err, rows, fields) {
			if (err)
					console.log("Something went wrong returning number of events. "+err);
			else {
				for (let row of rows){
					eventNum++;
				}
			}
			});

			connection.query(alarmCount, function (err, rows, fields) {
				if (err)
						console.log("Something went wrong with returning number of alarms. "+err);
				else {
					for (let row of rows){
						 	alarmNum++;
					}
					let string = "Database has " + fileNum + " files, " + eventNum + " events, and " + alarmNum + " alarms.";
					res.send(string);
				}
			});
});

app.get('/displaySortedEvents', function(req , res){

	let string = "SELECT * FROM EVENT ORDER BY start_time";
  let file = "";
	connection.query(string, function (err, result, fields) {
		if (err) {
				console.log("Something went wrong returning number of events. "+err);
		}
		else {
			res.send(result);
		}
	});


});

app.get('/displayEventFromFile', function(req , res){

	let fileNumber = req.query.fileNumber;

	let string = "SELECT event_id, summary, start_time FROM EVENT WHERE cal_file = " + fileNumber + ";"
	connection.query(string, function (err, result, fields) {
		if (err) {
				console.log("Something went wrong returning events from a specific file. "+err);
		}
		else {
			res.send(result);
		}
	});


});

app.get('/displayDoubleEvents', function(req , res){

	let string = "SELECT *  FROM EVENT WHERE summary IS NOT NULL && organizer IS NOT NULL && start_time IN (SELECT start_time FROM EVENT GROUP BY start_time HAVING COUNT(*) > 1);"
	connection.query(string, function (err, result, fields) {
		if (err) {
				console.log("Something went wrong returning events with the same start_time. "+err);
		}
		else {
			res.send(result);
		}
	});
});

app.get('/displayVersion20', function(req , res){

	let string = "SELECT * FROM ( FILE INNER JOIN EVENT ON EVENT.cal_file=FILE.cal_id ) WHERE version > 20;"
	connection.query(string, function (err, result, fields) {
		if (err) {
				console.log("Something went wrong returning events with version greater than 20. "+err);
		}
		else {
			res.send(result);
		}
	});
});

app.get('/averageEvents', function(req , res){

	let string = "SELECT DISTINCT s1.cal_file, (SELECT AVG(event_id) FROM EVENT s2 WHERE s2.cal_file = s1.cal_file) FROM EVENT s1";
	connection.query(string, function (err, result, fields) {
		if (err) {
				console.log("Something went wrong returning average of event id. "+err);
		}
		else {
			res.send(result);
		}
	});
});


app.get('/query4Alarm', function(req , res){

	let val = req.query.val;
	connection.query(val, function (err, result, fields) {
		if (err) {
				console.log("Something went wrong with running query 4. "+err);
		}
		else {
			res.send(result);
		}
	});


});

app.get('/query5MaxVersion', function(req , res){

	let realString;
	let val = req.query.val;

	let val1 = "SELECT *, MAX(version) AS version FROM FILE;";
	let val2 = "SELECT *, COUNT(version) AS version FROM FILE;";
	let val3 = "SELECT *, AVG(version) AS version FROM FILE;";
	let val4 = "SELECT *, SUM(version) AS version FROM FILE;";
	let val5 = "SELECT *, MIN(version) AS version FROM FILE;";

	if(val == val1) {
		realString = "SELECT *, MAX(version) AS max_version FROM FILE;";
	}
	else if(val == val2) {
		realString = "SELECT *, COUNT(version) AS count_version FROM FILE;";
	}
	else if(val == val3) {
		realString = "SELECT *, AVG(version) AS avg_version FROM FILE;";
	}
	else if(val == val4) {
		realString = "SELECT *, SUM(version) AS sum_version FROM FILE;";
	}
	else if(val == val5) {
		realString = "SELECT *, MIN(version) AS min_version FROM FILE;";
	}

	connection.query(realString, function (err, result, fields) {
		if (err) {
				console.log("Something went wrong with running query 5. "+err);
		}
		else {
			res.send(result);
		}
	});


});

app.get('/query6DuplicateVersion', function(req , res){

	let string = "SELECT * FROM FILE WHERE version IN (SELECT version FROM FILE GROUP BY version HAVING COUNT(*) > 1);"
	connection.query(string, function (err, result, fields) {
		if (err) {
				console.log("Something went wrong with running query 6. "+err);
		}
		else {
			res.send(result);
		}
	});
});
