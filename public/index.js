// Put all onload AJAX calls here, and event listeners
let svgList = Array(); //populate this dynamically...
let svgNames = Array();
let allSizes = [];
let list_of_num_rects = [];
let list_of_num_circs = [];
let list_of_num_paths = [];
let list_of_num_groups = [];
$(document).ready(function() {
    // On page-load AJAX Example
    document.getElementById("store_files").style.display="none";
    $.ajax({ //GETTING SVG FILES sizes
        type: 'get',
        dataType: 'json',
        url: '/fileSizes',
        /*data: {
            name: './uploads/'+x, //send svgList[value from selected dropdown... send in switchImage]
        },*/
        success: function(data) {
            //let temp = data.ok
            allSizes = data;
            if(allSizes.length > 0) {
                //give user the option to store all validated files if there is at least 1
                document.getElementById("store_files").style.display="block";
            }
        },
        fail: function(error) {
            alert("couldnt retrieve description");
            console.log(error);
            //console.log("Failed to get total number of rectangles from svg file");
        }
    });

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getFiles',   //The server endpoint we are connecting to
        data: {
            name1: "array"
        },
        success: function (data) {
            console.log("All files in uploads/ directory:" + data.foo);
            //We write the object to the console to show that the request was successful
            //for each file in array, add img src = array[i] to a new table row
            let i = 0;
            if(data.foo.length == 0) {
                $('<tr><td>No files to show</td></tr>').appendTo("#fileLog");
            }
            for(i = 0; i < data.foo.length; ++i) {
                let curFile = data.foo[i];
                //alert(i);
                let x = i;
                //if valid...
                let numRects = 0;
                let numCircs = 0;
                let numPaths = 0;
                let numGroups = 0;
                $.ajax({ //GETTING SVG FILES STATS
                    type: 'get',
                    dataType: 'json',
                    url: '/validateFile',
                    data: {
                        name: './uploads/'+curFile,
                    },
                    success: function(data) {
                        let temp = JSON.stringify(data.um);
                        let curFileSize = (parseFloat(allSizes[x])/1024).toFixed(0);
                        if(temp.localeCompare("{}") != 0) { //this means the svg was validated, and a json was returned
                            console.log(curFile + " has been successfully validated");
                            //populating file log panel
                            numRects = JSON.stringify(data.um.numRect);
                            list_of_num_rects[x] = numRects;
                            numCircs = JSON.stringify(data.um.numCirc);
                            list_of_num_circs[x] = numCircs;
                            numPaths = JSON.stringify(data.um.numPaths);
                            list_of_num_paths[x] = numPaths;
                            numGroups = JSON.stringify(data.um.numGroups);
                            list_of_num_groups[x] = numGroups;
                            $('<tr><td><a href='+curFile+' download='+curFile+'><img src='+curFile+' style="width:200px"></td><td><a href='+curFile+' download='+curFile+'>'+curFile+'</a>'
                            +'<td>'+curFileSize+'kb</td>' //FILE SIZE
                            +'<td>'+numRects+'</td>' //num of rectangles
                            +'<td>'+numCircs+'</td>' //num of circles
                            +'<td>'+numPaths+'</td>' //num of paths
                            +'<td>'+numGroups+'</td>' //num of groups
                            +'</tr>').appendTo("#fileLog");
                            //put file into svg dropdown
                            $('<option value='+x+'>'+curFile+'</option>').appendTo("#switch");
                            svgList[x] = new Image();
                            svgList[x].src = curFile;
                            svgNames[x] = new String();
                            svgNames[x] = curFile;
                        } else {
                            console.log(curFile + " could not be validated");
                            alert(curFile + " could not be validated");
                        }
                    },
                    fail: function(error) {
                        alert("Failed to validate ", curFile);
                        console.log("Failed to validate ", curFile);
                    }
                });
            }
            
        },
        fail: function(error) {
            // Non-200 return, do something with error
            alert("failed to get all filenames from uploads/");
            console.log("failed to get all filenames from uploads/");
            console.log(error); 
        }
    });

    $('#loginForm').submit(function(e){
        e.preventDefault();
        let username = $('#usernameEntry').val();
        let password = $('#passwordEntry').val();
        let dbname = $('#dbEntry').val();
        login(username, password, dbname);
    });

    $('#titleform').submit(function(e){
        $('#title').html($('#titleEntry').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
            //send to server to be validated,
            //if valid, change on actual svg image view
        });
    });



    $('#descform').submit(function(e){
        $('#desc').html($('#descEntry').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    $('#store_files_form').submit(function(e){
        //cf short for current file
        let cf_name;
        let cf_title;
        let cf_description;
        let cf_num_rect;
        let cf_num_circ;
        let cf_num_path;
        let cf_num_group;
        //let cf_create_date;
        let cf_size;
        //iterate thru every validated file
        //let i;
        for(let i = 0; i < svgNames.length; i++) {
            //put everything else other than title and description within a callback function? (function thats called after another function)
            $.ajax({ //GETTING SVG FILES STATS
                type: 'get',
                dataType: 'text',
                url: '/getTitle',
                data: {
                    name: './uploads/'+svgNames[i], //send svgList[value from selected dropdown... send in switchImage]
                },
                async: false,
                success: function(data) {

                    let title = data;
                    if(title.localeCompare("") == 0) { 
                        cf_title = "[No title]";
                    } else if(title.localeCompare("{}") != 0) { 
                        cf_title = title;
                    } else {
                        console.log(cf_name + ": could not retrieve title");
                    }
                },
                fail: function(error) {
                    alert(cf_name+ ": could not retrieve title");
                    console.log(error);
                }
            });

            $.ajax({ //GETTING SVG FILES STATS
                type: 'get',
                dataType: 'text',
                url: '/getDescription',
                data: {
                    name: './uploads/'+svgNames[i], //send svgList[value from selected dropdown... send in switchImage]
                },
                async: false,
                success: function(data) {
                    let desc = data;
                    if(desc.localeCompare("") == 0) { 
                        cf_description = "[No description]";
                    } else if(desc.localeCompare("{}") != 0) { 
                        cf_description = desc;
                    } else {
                        console.log(cf_name + ": could not retrieve description.");
                    }
                    
                },
                fail: function(error) {
                    alert(cf_name+ ": could not retrieve description.");
                    console.log(error);
                }
            });
            // alert("file name: " + cf_name + ", title: " + cf_title);
            // alert("num of rects: " + cf_num_rect);
            // alert("file size: " + cf_size + "b");
            cf_name = svgNames[i];
            cf_num_rect = list_of_num_rects[i];
            cf_num_circ = list_of_num_circs[i];
            cf_num_path = list_of_num_paths[i];
            cf_num_group = list_of_num_groups[i];
            cf_size = allSizes[i];
            // alert("file name: " + cf_name + ", title: " + cf_title + 
            // ", description: " + cf_description);
            // alert("num of rects: " + cf_num_rect);
            // alert("num of paths: " + cf_num_path);
            // alert("file size: " + cf_size + "b");
            storeAllFiles(cf_name, cf_num_rect, cf_num_circ, cf_num_path, cf_num_group, cf_size, cf_title, cf_description);
            
        }
        e.preventDefault();

    });
    /* 
    my stuff...
    */
    /*document.getElementById('upload-file').onclick = function () { //this isnt even used... i
        $('#status').html("User uploading file...");
    }; */

    document.getElementById('createSVG').onclick = function () { //this isnt even used... i
        $('#status').html("SVG creation in progress");
    };

    /*
    document.getElementById('addAttr').onclick = function () {
        $('#status').html("Adding attribute");
        //implement some sort of menu to take in name and value for an attribute

    }; */


});

//figure out how to dynamically load this array to hold all files within uploads...
//and update array when new files uploaded
function switchImage() {
    let selectedImage = document.svgDrop.switch.options[document.svgDrop.switch.selectedIndex].value;
    document.getElementById('myImage').src = svgList[selectedImage].src;
    let x = svgNames[selectedImage];
    loadTitle(x); //load in title
    loadDescription(x); //load in description
    //load in rows accord

    $("#svgViewBody tr").remove(); //reset stats for each new image picked
    //for every first layer rectangle, appendTo td="rectangle i", td = x, y, width + height, td = num of other attrs
    //need num of first layer rects: c function to return char num of getLength(image->rectangles)
    //need each first layer's rects JSON representation. get JSON array of all rects, can get length (numOfFirstLayerRects) from that
    //if json array=[], numRects = 0, so no iteration.

    $.ajax({ //GETTING SVG FILES STATS
        type: 'get',
        dataType: 'json',
        url: '/jsonFirstRects',
        data: {
            name: './uploads/'+x, //send svgList[value from selected dropdown... send in switchImage]
        },
        success: function(data) {
            //let temp = data.ok
            let JSONRects = data;
            let numRects = data.length;
            
            let z = 1;
            for(let i = 0; i < numRects; ++i) {
                let y = i + z;
                if(JSONRects[i].numAttr > 0) {
                    $('<tr><td>Rectangle '+y+'</td><td>Upper left corner: x = '+JSONRects[i].x+JSONRects[i].units+' , y = '+JSONRects[i].y+JSONRects[i].units+'<br>Width: '+JSONRects[i].w+JSONRects[i].units+', Height: '+JSONRects[i].h+JSONRects[i].units+'</td><td>'+JSONRects[i].numAttr+'<button type="button" id="'+i+'" value="'+x+'" name="1" class="btn btn-secondary" onclick="showAttribute(this.id,this.value,this.name)">Show other attributes</button></td>'
                    +'</tr>').appendTo("#svgViewBody");
                } else {
                    $('<tr><td>Rectangle '+y+'</td><td>Upper left corner: x = '+JSONRects[i].x+JSONRects[i].units+' , y = '+JSONRects[i].y+JSONRects[i].units+'<br>Width: '+JSONRects[i].w+JSONRects[i].units+', Height: '+JSONRects[i].h+JSONRects[i].units+'</td><td>'+JSONRects[i].numAttr+'</td>'
                    +'</tr>').appendTo("#svgViewBody");
                }
                
            }
            
        },
        fail: function(error) {
            //alert("couldnt retrieve title");
            alert("Failed to get array of first layer rectangles (JSON)");
            console.log("Failed to get array of first layer rectangles (JSON)");
        }
    });
    $.ajax({ //GETTING SVG FILES STATS
        type: 'get',
        dataType: 'json',
        url: '/jsonFirstCircs',
        data: {
            name: './uploads/'+x, //send svgList[value from selected dropdown... send in switchImage]
        },
        success: function(data) {
            let JSONCircs = data;
            let numCircs = data.length;
            let z = 1;
            for(let i = 0; i < numCircs; ++i) {
                let y = i + z;
                if(JSONCircs[i].numAttr > 0) {
                    $('<tr><td>Circle '+y+'</td><td>Centre: x = '+JSONCircs[i].cx+JSONCircs[i].units+' , y = '+JSONCircs[i].cy+JSONCircs[i].units+', radius: '+JSONCircs[i].r+JSONCircs[i].units+'</td><td>'+JSONCircs[i].numAttr+'<button type="button" id="'+i+'" value="'+x+'" name="2" class="btn btn-secondary" onclick="showAttribute(this.id,this.value,this.name)">Show other attributes</button></td>'
                    +'</tr>').appendTo("#svgViewBody");
                } else {
                    $('<tr><td>Circle '+y+'</td><td>Centre: x = '+JSONCircs[i].cx+JSONCircs[i].units+' , y = '+JSONCircs[i].cy+JSONCircs[i].units+', radius: '+JSONCircs[i].r+JSONCircs[i].units+'</td><td>'+JSONCircs[i].numAttr+'</td>'
                    +'</tr>').appendTo("#svgViewBody");
                }
                
            }
            
        },
        fail: function(error) {
            alert("Failed to get array of first layer circles (JSON)");
            console.log("Failed to get array of first layer circles (JSON)");
        }
    });
    $.ajax({ //GETTING SVG FILES STATS
        type: 'get',
        dataType: 'json',
        url: '/jsonFirstPaths',
        data: {
            name: './uploads/'+x, //send svgList[value from selected dropdown... send in switchImage]
        },
        success: function(data) {
            let JSONPaths = data;
            let numPaths = data.length;
            let z = 1;
            for(let i = 0; i < numPaths; ++i) {
                let y = i + z;
                if(JSONPaths[i].numAttr > 0) {
                    $('<tr><td>Path '+y+'</td><td>path data = '+JSONPaths[i].d+'</td><td>'+JSONPaths[i].numAttr+'<button type="button" id="'+i+'" value="'+x+'" name="3" class="btn btn-secondary" onclick="showAttribute(this.id,this.value,this.name)">Show other attributes</button></td>'
                    +'</tr>').appendTo("#svgViewBody");
                } else {
                    $('<tr><td>Path '+y+'</td><td>path data = '+JSONPaths[i].d+'</td><td>'+JSONPaths[i].numAttr+'</td>'
                    +'</tr>').appendTo("#svgViewBody");
                }
            }
            
        },
        fail: function(error) {
            alert("Failed to get array of first layer paths (JSON)");
            console.log("Failed to get array of first layer paths (JSON)");
        }
    });
    $.ajax({ //GETTING SVG FILES STATS
        type: 'get',
        dataType: 'json',
        url: '/jsonFirstGroups',
        data: {
            name: './uploads/'+x, //send svgList[value from selected dropdown... send in switchImage]
        },
        success: function(data) {
            let JSONGroups = data;
            let numGroups = data.length;
            let z = 1;
            for(let i = 0; i < numGroups; ++i) {
                let y = i + z;
                if(JSONGroups[i].numAttr > 0) {
                    $('<tr><td>Group '+y+'</td><td>'+JSONGroups[i].children+' child elements</td><td>'+JSONGroups[i].numAttr+'<button type="button" id="'+i+'" value="'+x+'" name="4" class="btn btn-secondary" onclick="showAttribute(this.id,this.value,this.name)">Show other attributes</button></td>'
                    +'</tr>').appendTo("#svgViewBody");
                } else {
                    $('<tr><td>Group '+y+'</td><td>'+JSONGroups[i].children+' child elements</td><td>'+JSONGroups[i].numAttr+'</td>'
                    +'</tr>').appendTo("#svgViewBody");
                }
                
            }
            
        },
        fail: function(error) {
            alert("Failed to get array of first layer groups (JSON)");
            console.log("Failed to get array of first layer groups (JSON)");
        }
    });
    /*$('<tr colspan="3"><td><button type="button" class="btn btn-secondary" onclick="userShape()">Add a shape</button></td>'
    +'</tr>').appendTo("#svgViewBody");*/

}
function userShape() {
    alert("Enter details of shape");

}
function showAttribute(id, fileName, num) {
    $("#otherAttrs tr").remove();
    $.ajax({ //GETTING SVG FILES STATS
        type: 'get',
        dataType: 'json',
        url: '/jsonAttrs',
        data: {
            name: './uploads/'+fileName, //send svgList[value from selected dropdown... send in switchImage]
            index: id,
            scenario : num
        },
        success: function(data) {
            let attrs = data;
            let numAttrs = data.length;
            let z = 1;
            for(let i = 0; i < numAttrs; ++i) {
                let y = i + z;
                $('<tr><td>Attribute '+y+' - name: '+ attrs[i].name +', value: ' + attrs[i].value+"</td></tr>").appendTo("#otherAttrs");
            }
            
        },
        fail: function(error) {
            alert("Failed to get other attributes information");
            console.log("Failed to get other attributes information");
        }
    });
   // $('#otherAttrs').html("current iteration " + id + " and the filename is " + fileName);
    //call function if button clicked..., send i, filename.
    //with filename, get list of first paths, get i'th path->otherAttributes...
    //call attrListToJSON(path[i]->otherAttributes), return list of that path's otherattrs
    //for returned json array returned, output name + value into div showAttrs
}


function loadTitle(x) {
    $.ajax({ //GETTING SVG FILES STATS
        type: 'get',
        dataType: 'text',
        url: '/getTitle',
        data: {
            name: './uploads/'+x, //send svgList[value from selected dropdown... send in switchImage]
        },
        success: function(data) {
            let title = data;
            if(title.localeCompare("") == 0) { 
                $('#title').html("[This SVG file has no title]");
            } else if(title.localeCompare("{}") != 0) { 
                $('#title').html(title);
            } else {
                console.log(curFile + ": could not retrieve title");
            }
        },
        fail: function(error) {
            alert(curFile+ ": could not retrieve title");
            //console.log("Failed to get total number of rectangles from svg file");
        }
    });
}

function loadDescription(x) {
    $.ajax({ //GETTING SVG FILES STATS
        type: 'get',
        dataType: 'text',
        url: '/getDescription',
        data: {
            name: './uploads/'+x, //send svgList[value from selected dropdown... send in switchImage]
        },
        success: function(data) {
            //let temp = data.ok
            let desc = data;
            if(desc.localeCompare("") == 0) { 
                $('#desc').html("[This SVG file has no description]");
            } else if(desc.localeCompare("{}") != 0) { 
                $('#desc').html(desc);
            } else {
                console.log(curFile + ": could not retrieve description");
            }
        },
        fail: function(error) {
            console.log("Failed to retrieve description");
            alert("Failed to retrieve description");
        }
    });
}

function login(user, pass, db) { //get these from html form? like title and description
    // code here to make rest of page invisible? (.hide()?)
    $.ajax({
        type: 'get',
        dataType: 'text',
        url: '/connectToDB',
        data: {
            username: user,
            password: pass,
            databasename : db
        },
        success: function(data) {
            //if data returned = 1, login successful, make rest of page visible
            //else, how can I send indicator back to web that login failed, and reprompt?
            //if(data != 0) {
            //alert("wtf");
            let stat = data;

            if(stat.localeCompare("yes") != 0) { 
                alert("Invalid credentials/database name, please try again.");
            }
            else {
                alert("Successfully logged in.");
                
            }
            //}
            
        },
        fail: function(error) {
            alert("Login failed");
            console.log("Login failed");
        }
    });
}

function storeAllFiles(f_name, f_num_rect, f_num_circ, f_num_path, f_num_group, f_size, f_title, f_desc) {
    $.ajax({
        type: 'get',
        data: 'text',
        url: '/storeFile',
        data: {
            name: f_name,
            rects: f_num_rect,
            circs: f_num_circ,
            paths: f_num_path,
            groups: f_num_group,
            size: f_size,
            title: f_title,
            desc: f_desc
        },
        success: function(data) {
            console.log("all files stored successfully...?");
        },
        fail: function(error) {
            console.log("Failed to store all files.\n" + error);
        }
    });
}
