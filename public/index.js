// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example

		var globalFiles = [];
		$.ajax({
				type: 'get',
				dataType: 'json',
				url: '/fileList',
				success: function (data) {
				for(var i = 0; i < data.length; i++) {
					globalFiles[i] = data[i];
				}
		},
		fail: function(error) {
				// Non-200 return, do something with error
				console.log(error);
		}
});//end jquery


$.ajax({
		type: 'get',
		dataType: 'json',
		url: '/invalidfileList',
		success: function (data) {
		for(var i = 0; i < data.length; i++) {
			sp("statusPanel","\n" +data[i]+ " in uploads is an invalid file");
			//$('#statusPanel').append("\n" +data[i]+ " in uploads is an invalid file\n");
			//globalFiles[i] = data[i];
		}
},
fail: function(error) {
		// Non-200 return, do something with error
		console.log(error);
}
});//end jquery


		$.ajax({
        type: 'get',
        dataType: 'json',
        url: '/object',
        success: function (data) {
					console.log(data.length);
					//globalFiles = data;
            for(i = 0; i<data.length; i++){
							//need to parse the file and get the rest of the data
							let string = "<tr><td><a href = \"/uploads/" + globalFiles[i] + "\">" + globalFiles[i] + "</a></td>"
												  + "<td>" +  data[i].version +  "</td>"
												  + "<td>" +  data[i].prodID +  "</td>"
												  + "<td>" +  data[i].numEvents +  "</td>"
												  + "<td>" +  data[i].numProps +  "</td></tr>"
								$('#tempFile').append(string);
							console.log(data[i]);
            }//end for
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });//end jquery

		let fname = "-1";
		var eSize;
		let clicked = false;
		$('#fileNames').on('click', function() {
			$('#addFiles').html("");
			let string = [];
			for(i = 0; i < globalFiles.length; i++) {
				let name = globalFiles[i];
				string[i] = $("<button class=\"btn btn-primary btn-lg sharp\">"+ globalFiles[i] + "</button>").on('click', function() {
					 $('#tempCal').html("");
					fname = name;
					$.ajax({
						type: 'get',            //Request type
						dataType: 'json',       //Data type - we will use JSON for almost everything
						url: '/tempCal',   //The server endpoint we are connecting to
						data: {
							filename:name,
						},
						success: function (data) {
							eSize = data.length;
							console.log(data);
							clicked = true; //MADE CLICK TRUE
							console.log("CLICKED IS: " + clicked);
							for(i = 0; i<data.length; i++){
								let num = i + 1;
								if(data[i].startDT.isUTC == true) {
									//console.log("EVER HERERE");
									var string = "<tr><td>" + num +  "</td>"
														 + "<td>" +  data[i].startDT.date +  "</td>"
														 + "<td>" +  data[i].startDT.time + "(UTC)"+"</td>"
														 + "<td>" +  data[i].summary +  "</td>"
														 + "<td>" +  data[i].numProps +  "</td>"
														 + "<td>" +  data[i].numAlarms +  "</td></tr>"
														 //console.log(data.filename);
									}
								else {
									//console.log("but here right...");
									var string = "<tr><td>" + num +  "</td>"
														 + "<td>" +  data[i].startDT.date +  "</td>"
														 + "<td>" +  data[i].startDT.time +  "</td>"
														 + "<td>" +  data[i].summary +  "</td>"
														 + "<td>" +  data[i].numProps +  "</td>"
														 + "<td>" +  data[i].numAlarms +  "</td></tr>"
														 //console.log(data.filename);
									}
									$('#tempCal').append(string);
								}
								sp("statusPanel","\nDisplaying the Events of file " + name);
							},
						fail: function(error) {
								// Non-200 return, do something with error
								console.log(error);
						}
					});
				});
			}
			$('#addFiles').append(string); //WHAT IS THIS DOING?
		});

    $('#someform').submit(function(e){ //DO WE NEED THIS?
        $('#blah').html("Callback from the form");
        e.preventDefault();
        $.ajax({});
    });

		/****** STATUS PANEL *****/
		$('#clearStatus').on('click', function() {
			$('#statusPanel').text('');
		});

		function sp(id, string){
			$('#' + id).append(string);
			var elem = document.getElementById('statusPanel');
			elem.scrollTop = elem.scrollHeight;
		}
		/****** STATUS PANEL *****/

		/******* DATABASE START*******/

		let loginSuccessful = false;

		$('#database').on('click', function() {
			let username  = $('#username').val();
			let password   = $('#password').val();
			//let database    = $('database').val();

			console.log(username + " " + password);
			if(username.length == 0 || password.length == 0) {
				sp("statusPanel","\nUnable to connect to the database. Fill in all the properties.");
				return;
			}

			$.ajax({
					type: 'get',
					//dataType: 'json',
					url: '/loginDatabase',
					data: {
						username:username,
						password:password,
					},
					success: function (data) {
						if(data == "DNE") {
							sp("statusPanel","\nUnable to connect to the database. Invalid username and/or password.");
							return;
						}
						else {
							loginSuccessful = true;
							sp("statusPanel","\nSuccessfully connected to the database.");
						}
			},
			fail: function(error) {
					// Non-200 return, do something with error
					console.log(error);
			}
		});//end jquery

		});

	//let numPressed = "yes";
let val = 0;
	$('#StoreAllFiles').on('click', function() {
		let doesExist = false;
		if(loginSuccessful == false) {
			sp("statusPanel","\nMust login before storing files.");
			return;
		}

						//data.length == globalFiles.length
						//console.log("table DNE");
						$.ajax({
								type: 'get',
								//dataType: 'json',
								url: '/StoreAllFiles',
								success: function (data) {

								console.log(data);
						},
						fail: function(error) {
								// Non-200 return, do something with error
								console.log(error);
						}
					});//end jquery
				//console.log(data);

		if(val > 0) {
			return;
		}
		val++;
});

	$('#ClearData').on('click', function() {
		if(loginSuccessful == false) {
			sp("statusPanel","\nMust login before clearing data.");
			return;
		}
		val--;

		$.ajax({
				type: 'get',
				//dataType: 'json',
				url: '/clearData',
				success: function (data) {

					console.log(data);
				},
				fail: function(error) {
					// Non-200 return, do something with error
					console.log(error);
				}
			});//end jquery
		});

		$('#DisplayDB').on('click', function() {
			if(loginSuccessful == false) {
				sp("statusPanel","\nMust login before displaying data.");
				return;
			}
			//alert("help");
			$.ajax({
					type: 'get',
					//dataType: 'json',
					url: '/DisplayDB',
					success: function (data) {
						sp("statusPanel","\n" + data);
						console.log(data);
					},
					fail: function(error) {
						// Non-200 return, do something with error
						console.log(error);
					}
				});//end jquery
			});

			//$("#btnSubmit").attr("disabled", true);

			$('#q1').on('click', function() {

				if(loginSuccessful == false) {
					sp("statusPanel","\nMust login before running a query.");
					return;
				}

				$("#printQuery").val("SELECT * FROM EVENT ORDER BY start_time;");
				$("#queryMessage").text("Cannot change this query.");

				$("#tempDataBase tr").remove();

				$("#message").text("");

				$('#btnSubmit').on('click', function() {
					//event.preventDefault();
					$("#tempDataBase tr").remove();
					let q1Val = $('#printQuery').val();

					if(q1Val != "SELECT * FROM EVENT ORDER BY start_time;") {
						console.log("here here 1");
						$("#message").text("Invalid query to display events ordered by start_time.");
						return;
					}
					else{
						$("#message").text("");
					}

					$.ajax({
							type: 'get',
							//dataType: 'json',
							url: '/displaySortedEvents',
							success: function (data) {
								$("#tempDataBase tr").remove();
								let firstString = "<tr><td>" + "event_id" + "</td>"
														+ "<td>" +  "summary" +  "</td>"
														+ "<td>" +  "start_time" +  "</td>"
														+ "<td>" +  "location" +  "</td>"
														+ "<td>" +  "organizer" +  "</td>"
														+ "<td>" +  "cal_file" +  "</td></tr>"
														$('#tempDataBase').append(firstString);
								//console.log(data);
								for(i = 0; i<data.length; i++){
									//need to parse the file and get the rest of the data
									let string = "<tr><td>" + data[i].event_id + "</td>"
															+ "<td>" +  data[i].summary +  "</td>"
															+ "<td>" +  data[i].start_time +  "</td>"
															+ "<td>" +  data[i].location +  "</td>"
															+ "<td>" +  data[i].organizer +  "</td>"
															+ "<td>" +  data[i].cal_file +  "</td></tr>"
															$('#tempDataBase').append(string);
										//$('#tempFile').append(string);
									console.log(data[i]);
								}//end for
							},
							fail: function(error) {
								// Non-200 return, do something with error
								console.log(error);
							}
						});//end jquery
				});
			});

			$('#q2').on('click', function() {
				if(loginSuccessful == false) {
					sp("statusPanel","\nMust login before running a query.");
					return;
				}
				//DID NOT SORT
				let fileName   = fname;
				let fileNumber = 0;
				let selected   = false;

				$("#printQuery").val('Enter file name then press execute query: ');

				$("#queryMessage").text("");

				$("#tempDataBase tr").remove();

				$("#message").text("");

				$('#btnSubmit').on('click', function() {

					$("#tempDataBase tr").remove();
					let q2Val = $('#printQuery').val();
					let fileNumber;
					let sub;
					let varSub = false;
					for(let i = 0; i < globalFiles.length; i++) {
						sub = q2Val.includes(globalFiles[i]);
					 	 if(sub) {
							 varSub = true;
							 fileNumber = i + 1;
						 }
					}

					if(varSub == false) {
						console.log("here here 2");
						$("#message").text("File does not exist.");
						return;
					}
					else{
						$("#message").text("");
					}

					$.ajax({
							type: 'get',
							//dataType: 'json',
							url: '/displayEventFromFile',
							data: {
								fileNumber:fileNumber,
							},
							success: function (data) {
								$("#tempDataBase tr").remove();
								let firstString = "<tr><td>" + "event_id" + "</td>"
																	+ "<td>" +  "summary" +  "</td>"
																	+ "<td>" + "start_time" +  "</td></tr>"
																	$('#tempDataBase').append(firstString);

											for(i = 0; i<data.length; i++){
												//need to parse the file and get the rest of the data
												let string = "<tr><td>" + data[i].event_id + "</td>"
																		+ "<td>" +  data[i].summary +  "</td>"
																		+ "<td>" +  data[i].start_time +  "</td></tr>"
																		$('#tempDataBase').append(string);
											}//end for
							},
							fail: function(error) {
								// Non-200 return, do something with error
								console.log(error);
							}
						});//end jquery
				});
			});

			$('#q3').on('click', function() {
				if(loginSuccessful == false) {
					sp("statusPanel","\nMust login before running a query.");
					return;
				}

				$("#printQuery").val("SELECT *  FROM EVENT WHERE summary IS NOT NULL && organizer IS NOT NULL && start_time IN (SELECT start_time FROM EVENT GROUP BY start_time HAVING COUNT(*) > 1);");

				$("#tempDataBase tr").remove();

				$("#queryMessage").text("Cannot change this query.");

				$("#message").text("");

				$('#btnSubmit').on('click', function() {

					//event.preventDefault();
					let q3Val  = $('#printQuery').val();
					let q3Val1 = "SELECT *  FROM EVENT WHERE summary IS NOT NULL && organizer IS NOT NULL && start_time IN (SELECT start_time FROM EVENT GROUP BY start_time HAVING COUNT(*) > 1);";

					if(q3Val != "SELECT *  FROM EVENT WHERE summary IS NOT NULL && organizer IS NOT NULL && start_time IN (SELECT start_time FROM EVENT GROUP BY start_time HAVING COUNT(*) > 1);") {
						$("#message").text("Invalid query to display events which overlap.");
						console.log("here here 3");
						return;
					}
					else {
						$('#message').text("");
					}

				$.ajax({
						type: 'get',
						//dataType: 'json',
						url: '/displayDoubleEvents',
						success: function (data) {
							$("#tempDataBase tr").remove();
							let firstString = "<tr><td>" + "event_id" + "</td>"
													+ "<td>" +  "summary" +  "</td>"
													+ "<td>" +  "start_time" +  "</td>"
													+ "<td>" +  "location" +  "</td>"
													+ "<td>" +  "organizer" +  "</td>"
													+ "<td>" +  "cal_file" +  "</td></tr>"
													$('#tempDataBase').append(firstString);
							//console.log(data);
							for(i = 0; i<data.length; i++){
								//need to parse the file and get the rest of the data
								let string = "<tr><td>" + data[i].event_id + "</td>"
														+ "<td>" +  data[i].summary +  "</td>"
														+ "<td>" +  data[i].start_time +  "</td>"
														+ "<td>" +  data[i].location +  "</td>"
														+ "<td>" +  data[i].organizer +  "</td>"
														+ "<td>" +  data[i].cal_file +  "</td></tr>"
													  $('#tempDataBase').append(string);
									//$('#tempFile').append(string);
								console.log(data[i]);
							}//end for
						},
						fail: function(error) {
							// Non-200 return, do something with error
							console.log(error);
						}
					});//end jquery
			});
		});

			$('#q4').on('click', function() {
				//event.preventDefault();
				if(loginSuccessful == false) {
					sp("statusPanel","\nMust login before running a query.");
					return;
				}

				$("#printQuery").val("SELECT event_id, summary, start_time, location, organizer, alarm_id, action, `trigger`  FROM EVENT INNER JOIN ALARM ON ALARM.event=EVENT.event_id;");
				//$('#printQuery').text("SELECT *, MAX(version) AS max_version FROM FILE;");
				$("#tempDataBase tr").remove();

				$("#queryMessage").text("Can change INNER to either LEFT or RIGHT.");

				$("#message").text("");

				$('#btnSubmit').on('click', function() {

					//event.preventDefault();
					$("#tempDataBase tr").remove();
					let q4Val = $('#printQuery').val();

					let q4Val1 = "SELECT event_id, summary, start_time, location, organizer, alarm_id, action, `trigger`  FROM EVENT LEFT JOIN ALARM ON ALARM.event=EVENT.event_id;";
					let q4Val2 = "SELECT event_id, summary, start_time, location, organizer, alarm_id, action, `trigger`  FROM EVENT RIGHT JOIN ALARM ON ALARM.event=EVENT.event_id;";
					let q4Val3 = "SELECT event_id, summary, start_time, location, organizer, alarm_id, action, `trigger`  FROM EVENT INNER JOIN ALARM ON ALARM.event=EVENT.event_id;";

					if(q4Val == q4Val1) {
						//
					}
					else if(q4Val == q4Val2) {
						//
					}
					else if(q4Val == q4Val3) {
						//
					}
					else{
						console.log("here here 4");
						$("#message").text("Invalid query to display all alarms.");
						return;
					}

					$("#message").text("");

				$.ajax({
						type: 'get',
						//dataType: 'json',
						url: '/query4Alarm',
						data: {
							val:q4Val,
						},
			  		success: function (data) {
							$("#tempDataBase tr").remove();
									let firstString = "<tr><td>" + "event_id" + "</td>"
															+ "<td>" +  "summary" +  "</td>"
															+ "<td>" +  "start_time" +  "</td>"
															+ "<td>" +  "location" +  "</td>"
															+ "<td>" +  "organizer" +  "</td>"
															+ "<td>" +  "alarm_id" +  "</td>"
															+ "<td>" +  "action" +  "</td>"
															+ "<td>" +  "trigger" +  "</td></tr>"
															$('#tempDataBase').append(firstString);
								for(i = 0; i<data.length; i++){
									//need to parse the file and get the rest of the data
									let string = "<tr><td>" + data[i].event_id + "</td>"
															+ "<td>" +  data[i].summary +  "</td>"
															+ "<td>" +  data[i].start_time +  "</td>"
															+ "<td>" +  data[i].location +  "</td>"
															+ "<td>" +  data[i].organizer +  "</td>"
															+ "<td>" +  data[i].alarm_id +  "</td>"
															+ "<td>" +  data[i].action +  "</td>"
															+ "<td>" +  data[i].trigger +  "</td></tr>"
															$('#tempDataBase').append(string);
								}
							//}
							//event.preventDefault();
							return;
						},//end for
						fail: function(error) {
							// Non-200 return, do something with error
							console.log(error);
						}
					});//end jquery
				});
			});

			$('#q5').on('click', function() {
				//event.preventDefault();
				if(loginSuccessful == false) {
					sp("statusPanel","\nMust login before running a query.");
					return;
				}
				$("#printQuery").val("SELECT *, MAX(version) AS version FROM FILE;");
				$("#tempDataBase tr").remove();

				$("#queryMessage").text("Can change max to one of the other aggregate functions.");

				$("#message").text("");

				$('#btnSubmit').on('click', function() {

				$("#tempDataBase tr").remove();

					let q5Val = $('#printQuery').val();

					let q5Val1 = "SELECT *, MAX(version) AS version FROM FILE;";
					let q5Val2 = "SELECT *, COUNT(version) AS version FROM FILE;";
					let q5Val3 = "SELECT *, AVG(version) AS version FROM FILE;";
					let q5Val4 = "SELECT *, SUM(version) AS version FROM FILE;";
					let q5Val5 = "SELECT *, MIN(version) AS version FROM FILE;";

					if(q5Val == q5Val1) {
						///
					}
					else if(q5Val == q5Val2) {
						//
					}
					else if(q5Val == q5Val3) {
						//
					}
					else if(q5Val == q5Val4) {
						//
					}
					else if(q5Val == q5Val5) {
						//
					}
					else {
						console.log("here here 5");
						$("#message").text("Invalid query.");
						return;
					}

					$('#message').text("");

				$.ajax({
						type: 'get',
						//dataType: 'json',
						url: '/query5MaxVersion',
						data: {
							val:q5Val,
						},
						success: function (data) {
							$("#tempDataBase tr").remove();
							if(q5Val == q5Val1) {
								let firstString = "<tr><td>" + "cal_id" + "</td>"
														+ "<td>" +  "file_Name" +  "</td>"
														+ "<td>" +  "version" +  "</td>"
														+ "<td>" +  "prod_id" +  "</td>"
														+ "<td>" +  "max_version" +  "</td></tr>"
														$('#tempDataBase').append(firstString);

								for(i = 0; i<data.length; i++){
									//need to parse the file and get the rest of the data
									let string = "<tr><td>" + data[i].cal_id + "</td>"
										+ "<td>" +  data[i].file_Name +  "</td>"
										+ "<td>" +  data[i].version +  "</td>"
									  + "<td>" +  data[i].prod_id +  "</td>"
										+ "<td>" +  data[i].max_version +  "</td></tr>"
									  $('#tempDataBase').append(string);
										//console.log(data[i]);
							 }//end for
							}
							else if(q5Val == q5Val2) {
								let firstString = "<tr><td>" + "cal_id" + "</td>"
														+ "<td>" +  "file_Name" +  "</td>"
														+ "<td>" +  "version" +  "</td>"
														+ "<td>" +  "prod_id" +  "</td>"
														+ "<td>" +  "count_version" +  "</td></tr>"
														$('#tempDataBase').append(firstString);

								for(i = 0; i<data.length; i++){
									//need to parse the file and get the rest of the data
									let string = "<tr><td>" + data[i].cal_id + "</td>"
										+ "<td>" +  data[i].file_Name +  "</td>"
										+ "<td>" +  data[i].version +  "</td>"
									  + "<td>" +  data[i].prod_id +  "</td>"
										+ "<td>" +  data[i].count_version +  "</td></tr>"
									  $('#tempDataBase').append(string);
										//console.log(data[i]);
							 }//end for
							}
							else if(q5Val == q5Val3) {
								let firstString = "<tr><td>" + "cal_id" + "</td>"
														+ "<td>" +  "file_Name" +  "</td>"
														+ "<td>" +  "version" +  "</td>"
														+ "<td>" +  "prod_id" +  "</td>"
														+ "<td>" +  "avg_version" +  "</td></tr>"
														$('#tempDataBase').append(firstString);

								for(i = 0; i<data.length; i++){
									//need to parse the file and get the rest of the data
									let string = "<tr><td>" + data[i].cal_id + "</td>"
										+ "<td>" +  data[i].file_Name +  "</td>"
										+ "<td>" +  data[i].version +  "</td>"
									  + "<td>" +  data[i].prod_id +  "</td>"
										+ "<td>" +  data[i].avg_version +  "</td></tr>"
									  $('#tempDataBase').append(string);
										//console.log(data[i]);
							 }//end for
							}
							else if(q5Val == q5Val4) {
								let firstString = "<tr><td>" + "cal_id" + "</td>"
														+ "<td>" +  "file_Name" +  "</td>"
														+ "<td>" +  "version" +  "</td>"
														+ "<td>" +  "prod_id" +  "</td>"
														+ "<td>" +  "sum_version" +  "</td></tr>"
														$('#tempDataBase').append(firstString);

								for(i = 0; i<data.length; i++){
									//need to parse the file and get the rest of the data
									let string = "<tr><td>" + data[i].cal_id + "</td>"
										+ "<td>" +  data[i].file_Name +  "</td>"
										+ "<td>" +  data[i].version +  "</td>"
									  + "<td>" +  data[i].prod_id +  "</td>"
										+ "<td>" +  data[i].sum_version +  "</td></tr>"
									  $('#tempDataBase').append(string);
										//console.log(data[i]);
							 }//end for
							}
							else if(q5Val == q5Val5) {
								let firstString = "<tr><td>" + "cal_id" + "</td>"
														+ "<td>" +  "file_Name" +  "</td>"
														+ "<td>" +  "version" +  "</td>"
														+ "<td>" +  "prod_id" +  "</td>"
														+ "<td>" +  "min_version" +  "</td></tr>"
														$('#tempDataBase').append(firstString);

								for(i = 0; i<data.length; i++){
									//need to parse the file and get the rest of the data
									let string = "<tr><td>" + data[i].cal_id + "</td>"
										+ "<td>" +  data[i].file_Name +  "</td>"
										+ "<td>" +  data[i].version +  "</td>"
									  + "<td>" +  data[i].prod_id +  "</td>"
										+ "<td>" +  data[i].min_version +  "</td></tr>"
									  $('#tempDataBase').append(string);
										//console.log(data[i]);
							 }//end for
							}
						//
			 		},
					fail: function(error) {
						// Non-200 return, do something with error
						console.log(error);
					}
				});//end jquery

			});
		});

			$('#q6').on('click', function() {

				if(loginSuccessful == false) {
					sp("statusPanel","\nMust login before running a query.");
					return;
				}
				//alert("help");
				console.log("design whatever = " + $("#printQuery").val());
				$("#printQuery").val("SELECT * FROM FILE WHERE version IN (SELECT version FROM FILE GROUP BY version HAVING COUNT(*) > 1);");

				$("#tempDataBase tr").remove();

				$("#queryMessage").text("Cannot change this query.");

				$("#message").text("");

				$('#btnSubmit').on('click', function() {


					$("#tempDataBase tr").remove();
					let q6Val  = $('#printQuery').val();

					let q6Val1 = "SELECT * FROM FILE WHERE version IN (SELECT version FROM FILE GROUP BY version HAVING COUNT(*) > 1);";

					if(q6Val != q6Val1) {
						console.log("here here 6");
						$("#message").text("Invalid query, will not display files with duplicate versions.");
						return;
					}
					else{
						$("#message").text("");
					}

					console.log("what is val = " + q6Val);

				$.ajax({
						type: 'get',
						//dataType: 'json',
						url: '/query6DuplicateVersion',
						success: function (data) {
							$("#tempDataBase tr").remove();
							let firstString = "<tr><td>" + "cal_id" + "</td>"
													+ "<td>" +  "file_Name" +  "</td>"
													+ "<td>" +  "version" +  "</td>"
													+ "<td>" +  "prod_id" +  "</td></tr>"
													$('#tempDataBase').append(firstString);

							for(i = 0; i<data.length; i++){
								//need to parse the file and get the rest of the data
								let string = "<tr><td>" + data[i].cal_id + "</td>"
									+ "<td>" +  data[i].file_Name +  "</td>"
									+ "<td>" +  data[i].version +  "</td>"
								  + "<td>" +  data[i].prod_id +  "</td></tr>"
								  $('#tempDataBase').append(string);
									//console.log(data[i]);
						 }//end for
			 		},
					fail: function(error) {
						// Non-200 return, do something with error
						console.log(error);
					}
				});//end jquery
			});
		});

		/******* DATABASE END*******/

		/****** FILE PANEL *****/
		$('#add-row').on('click', function() {

			let startDate = getCurrentDate();
			let startTime = getCurrentTime();

			let fileName = $('#fileName').val(); //make sure it ends in .ics
			let version  = $('#version').val();
			let prodID   = $('#prodID').val();
			let evUID    = $('#evUID').val();

			let string = fileName;
    	let substring = ".ics";

			if(string.includes(substring) == false) {
				sp("statusPanel","\nFile name must end with .ics!");
				//$('#statusPanel').append("\nFile name must end with .ics!\n");
				return;
			};

			if(Number.isInteger(+version) == false) {
				sp("statusPanel","\nVersion must be a number!");
				//$('#statusPanel').append("\nVersion must be a number!\n");
				return;
			}
			//fil.indexOf('word') !== -1

			if(fileName == "") {
				sp("statusPanel","\nFile Name cannot be empty!");
				//$('#statusPanel').append("\nFile Name cannot be empty!\n");
				return;
			}

			if(version == "") {
				sp("statusPanel","\nVersion cannot be empty!");
				//$('#statusPanel').append("\nVersion cannot be empty!\n");
				return;
			}

			if(prodID == "") {
				sp("statusPanel","\nProduct ID cannot be empty!");
				//$('#statusPanel').append("\nProduct ID cannot be empty!\n");
				return;
			}

			if(evUID == "") {
				sp("statusPanel","\nEvent ID cannot be empty!");
				//$('#statusPanel').append("\nEvent ID cannot be empty!\n");
				return;
			}

		$.ajax({
			type: 'get',            //Request type
			dataType: 'json',       //Data type - we will use JSON for almost everything
			url: '/fileCreated',   //The server endpoint we are connecting to
			data: {
				fileName:fileName,
				version:version,
				prodID:prodID,
				evUID:evUID,
				startDate:startDate,
				startTime:startTime,
			},
			success: function (data) {
					var string = "<tr><td><a href = \"/uploads/" + fileName + "\">" +  fileName  + "</a></td>"
					           + "<td>" +  data.version +  "</td>"
										 + "<td>" +  data.prodID +  "</td>"
										 + "<td>" +  data.numProps +  "</td>"
										 + "<td>" +  data.numEvents +  "</td></tr>"
						$('#tempFile').append(string);
						sp("statusPanel","\nAdding New File called " + fileName + "... please refresh the page.");
						//$('#statusPanel').append("\nAdding New File called " + fileName + "\n");
				},
			fail: function(error) {
					// Non-200 return, do something with error
					console.log(error);
			}
		});
});

function getCurrentTime(){
	let d = new Date; // for now
  var hours   = d.getHours(); // => 9
  var minutes = d.getMinutes(); // =>  30
  var seconds = d.getSeconds();


			if(hours < 10) {
					hours = '0'+hours;
			}

			if(hours == 0) {
				hours = '00';
			}

			if(minutes < 10) {
					minutes = '0'+minutes;
			}
			if(minutes == 0) {
				 minutes = '00';
			}

			if(seconds < 10) {
					seconds = '0'+seconds;
			}
			if(seconds == 0) {
				 seconds = '00';
			}

	let fullTime = hours + '' + minutes + '' + seconds;
	console.log(fullTime);
	return fullTime;
}


function getCurrentDate(){
		let currDate = new Date();
		var day = currDate.getDate();
		var month = currDate.getMonth()+1; //Months a 0-indexed
		var year = currDate.getFullYear();

		if(day < 10) {
				day = '0'+day;
		}

		if(month < 10) {
				month = '0'+month;
		}

		let fullDate = year + month + day;
		return fullDate;
}
	//let totalLength =
	$('#createEvent').on('click', function() {

		if(clicked == false) {
			console.log("CANNOT ADD EVENT BECAUSE FILE NOT SELECTED");
			sp("statusPanel","\nFirst select a file to add an event to it");
			//$('#statusPanel').append("\nFirst select a file to add an event to it\n");
			return;
		}

		let uid = $('#UID').val();
		let summary = $('#summary').val();

		let evStartDate = $('#evStartDate').val();
		let evStartTime = $('#evStartTime').val();

		evStartDate = evStartDate.replace('-','');
		evStartDate = evStartDate.replace('-','');

		console.log(evStartDate);

		if(uid == "") {
			sp("statusPanel","\nUID cannot be empty!");
			//$('#statusPanel').append("\nUID cannot be empty!\n");
			return;
		}

		if(evStartDate == "" || evStartTime == "") {
			sp("statusPanel","\nEvent start date and start time both must be filled!");
			//$('#statusPanel').append("\nEvent start date and start time both must be filled!\n");
			return;
		}

		console.log(evStartTime.length);

		if(evStartTime.length !== 6) {
			sp("statusPanel","\nInvalid Start Time Input -- input must be HHMMSS (HOURS/MINUTES/SECONDS)!");
			//$('#statusPanel').append("\nInvalid Start Time Input -- input must be HHMMSS!\n");
			return;
		}

		if(isNaN(evStartTime) == true) {
			sp("statusPanel","\nInvalid Start Time Input -- input must be a number!");
			//$('#statusPanel').append("\nInvalid Start Time Input -- input must be a number!\n");
			return;
		}

		if(summary == "") {
			summary = "DNE";
		}

		let utc;
		if($('#utcSelected').prop('checked')) {
			//console.log("does this even maek sense??");
    	// something when checked
			utc = "true";
		} else {
			//console.log("nonopppppppeee?");
    	// something else when not
			utc = "false";
		}

		let startDate = getCurrentDate();
		let startTime = getCurrentTime();

		let filename = fname;

		$.ajax({
			type: 'get',            //Request type
			dataType: 'json',       //Data type - we will use JSON for almost everything
			url: '/addEvent',   //The server endpoint we are connecting to
			data: {
				filename:filename,
				uid:uid,
				summary:summary,
				startDate:startDate,
				startTime:startTime,
				evstartDate:evStartDate,
				evstartTime:evStartTime,
				utc:utc,
			},
			success: function (data) {
				console.log(data);
					var num = eSize + 1;
					eSize = eSize + 1;

					if(utc == "true") {
						var string = "<tr><td>" +  num +  "</td>"
											 + "<td>" +  data.startDT.date +  "</td>"
											 + "<td>" +  data.startDT.time +  "(UTC)</td>"
											 + "<td>" +  data.summary +  "</td>"
											 + "<td>" +  data.numProps +  "</td>"
											 + "<td>" +  data.numAlarms +  "</td></tr>"
					}
					else{
						var string = "<tr><td>" +  num +  "</td>"
											 + "<td>" +  data.startDT.date +  "</td>"
											 + "<td>" +  data.startDT.time +  "</td>"
											 + "<td>" +  data.summary +  "</td>"
											 + "<td>" +  data.numProps +  "</td>"
											 + "<td>" +  data.numAlarms +  "</td></tr>"
					}
										 $('#tempCal').append(string);
										 sp("statusPanel","\nAdding Calendar Event to " + filename);
										 //$('#statusPanel').append("\nAdding Calendar Event to " + filename + "\n");
				},

			fail: function(error) {
					// Non-200 return, do something with error
					console.log(error);
			}
		});
	});

$('#showAlarms').on('click', function() {

	if(clicked == false) {
		console.log("CANNOT CLICK SHOW ALARMS BECAUSE FILE NOT SELECTED");
		sp("statusPanel","\nSelect an event to see its alarms");
		return;
	}

	 let num   = $('#eventNumForAlarm').val();
	 let filename = fname;

	 if(num > eSize) {
		 sp("statusPanel","\nNO EVENT AT NUMBER " + num);
		 return;
	 }

	 if(num == '') {
		 console.log("NO NUMBER ENTERED!");
		 sp("statusPanel","\nEnter the number of the event to see its alarms");
		 return;
	 }

	 $.ajax({
		 type: 'get',            //Request type
		 dataType: 'json',       //Data type - we will use JSON for almost everything
		 url: '/displayAlarm',   //The server endpoint we are connecting to
		 data: {
			 filename:filename,
			 num:num,
		 },
		 success: function (data) {
			 let string = "\nAlarms in event " + num + " in file " + filename;

			 if(data.action == "DNE") {
				 string = string + "\nNO ALARMS";
				 console.log("NO ALARMS");
			 }
			 else{
				 for(i = 0; i < data.length; i++) {
					 string = string + "\nAlarm " +(i + 1)+ ":"+"\nAction = " + data[i].action + "\n"
												+ "Trigger = " + data[i].trigger + "\n"
												+ "Number of Properties = " + data[i].numProps
				 }
			 }
			 sp("statusPanel",string);
		 },
	   fail: function(error) {
			 // Non-200 return, do something with error
			 console.log(error);
	 }
 });
});

$('#showProps').on('click', function() {
	if(clicked == false) {
		console.log("CANNOT CLICK SHOW PROPS BECAUSE FILE NOT SELECTED");
		sp("statusPanel","\nSelect an event to see its properties");
		return;
	}

	 let num   = $('#eventNumForAlarm').val();
	 //console.log("WHAT IS NUM: " + num);
	 let filename = fname;

	 if(num > eSize) {
		 sp("statusPanel","\nNO EVENT AT NUMBER " + num);
		//$('#statusPanel').append("\nNO EVENT AT NUMBER " + num + "\n");
		return;
	}

	//IF NUM IS EMPTY
	if(num == '') {
		console.log("NO NUMBER ENTERED!");
		sp("statusPanel","\nEnter the number of the event to see its properties");
		//$('#statusPanel').append("\nEnter the number of the event to see its properties\n");
		return;
	}

	 $.ajax({
		 type: 'get',            //Request type
		 dataType: 'json',       //Data type - we will use JSON for almost everything
		 url: '/displayProp',   //The server endpoint we are connecting to
		 data: {
			 filename:filename,
			 num:num,
		 },
		 success: function (data) {
			 let string = "\nProperties in event " + num + " in file " + filename +  ":";

			 if(data.propName == "DNE") {
				 string = string + "\nNO OPTIONAL PROPERTIES";
				 console.log("IS IT UNDEFIEND");
			 }
			 else{
				 for(i = 0; i < data.length; i++) {
					 string += "\nProperty " +(i + 1)+ ":" + "\nProperty Name: " + data[i].propName + ",   Property Description: " + data[i].propDescr;
				 }
				 console.log("DEFINED");
			 }
			 sp("statusPanel",string);
		 },
	   fail: function(error) {
			 // Non-200 return, do something with error
			 console.log(error);
	 }
 });
});

});
