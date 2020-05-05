'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// testing dummy connection to server
let svgparser = ffi.Library('./libsvgparse', {
    'createSVGFileFromJSON': [ 'void', ['string','string'] ],		//return type first, argument list second
    'jsonOfCreateValidSVGimage': ['string', ['string']],
    'getTitle' : ['string', ['string']],
    'getDescription' : ['string', ['string']],
    'JSONfirstLayerRects' : ['string', ['string']],
    'JSONfirstLayerCircs' : ['string', ['string']],
    'JSONfirstLayerPaths' : ['string', ['string']],
    'JSONfirstLayerGroups' : ['string', ['string']],
    'JSONattrsListOfCurrentThing' : ['string', ['string', 'int', 'int']]
});


// Send HTML at root, do not change
app.get('/',function(req,res){
    res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
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
        if(err == null) {
            res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
        } else {
            console.log('Error in file downloading route: '+err);
            res.send('');
        }
    });
});
//******************** Your code goes here ******************** 

app.get('/getFiles', function(req , res){
    let allFiles = fs.readdirSync("uploads/");
    //console.log("All files loaded: " + allFiles);
    res.send({
        foo: allFiles
    });
});

app.get('/validateFile', function(req, res){
    let ok = svgparser.jsonOfCreateValidSVGimage(req.query.name);
    let um = JSON.parse(ok);
    res.send({
        um
    });
});

app.get('/getTitle', function(req, res){
    let ok = svgparser.getTitle(req.query.name);
    res.send(ok);
});

app.get('/getDescription', function(req, res){
    let ok = svgparser.getDescription(req.query.name);
    res.send(ok);
});

app.get('/getTitleAndDescription', function(req, res){
    let title = svgparser.getTitle(req.query.name);
    let desc = svgparser.getDescription(req.query.name);
    res.send({
        title,
        desc
    });
});


app.get('/fileSizes', function(req, res){
    let allFiles = fs.readdirSync("uploads/");
    let sizes = [];
    allFiles.forEach(file => {
        //if valid...
        let stats = fs.statSync("uploads/"+file);
        //console.log("THE FILE NAME is "+file);
        let valid = svgparser.jsonOfCreateValidSVGimage("./uploads/"+file); //figure out to validate
        if(valid.localeCompare("{}") != 0) { //if valid file add file size
            let size = stats.size;
            sizes.push(size);
        }
    }); //iterate thru files, store sizes
    res.send(sizes);
});

app.get('/jsonFirstRects', function(req, res){
    let ok = svgparser.JSONfirstLayerRects(req.query.name);
    let ret = JSON.parse(ok);
    res.send(ret); //should return JSON array of first layer rects
});

app.get('/jsonFirstCircs', function(req, res){
    let ok = svgparser.JSONfirstLayerCircs(req.query.name);
    let ret = JSON.parse(ok);
    res.send(ret); //should return JSON array of first layer rects
});

app.get('/jsonFirstPaths', function(req, res){
    let ok = svgparser.JSONfirstLayerPaths(req.query.name);
    let ret = JSON.parse(ok);
    res.send(ret); //should return JSON array of first layer rects
});

app.get('/jsonFirstGroups', function(req, res){
    let ok = svgparser.JSONfirstLayerGroups(req.query.name);
    let ret = JSON.parse(ok);
    res.send(ret); //should return JSON array of first layer rects
});

app.get('/jsonAttrs', function(req, res){
    let ok = svgparser.JSONattrsListOfCurrentThing(req.query.name, req.query.index, req.query.scenario);
    let ret = JSON.parse(ok);
    res.send(ret); //should return JSON array of current path's otherAttrs
});

let connection;
let dbCredentials;
const mysql = require('mysql2/promise');
app.get('/connectToDB', async function(req, res) {
    //take req.query.username, .password .databasename, try to connect to database, catch exception
    let good = "yes";
    //const mysql = require('mysql2/promise');
    dbCredentials = {
        host : 'dursley.socs.uoguelph.ca',
        user : req.query.username,
        password : req.query.password,
        database : req.query.databasename
    };

    try {
        connection = await mysql.createConnection(dbCredentials);
        console.log("Connected to database " + req.query.databasename);
        //...
        await createTables();
        //if successful, check if tables created, if not, create them?

    } catch(e) {
        good = "no";
        console.log("Query error: "+e);
        //res.send(1);
    } finally {
        if (connection && connection.end) {
            connection.end();
            console.log("Disconnected from database " + req.query.databasename);
        }
        res.send(good);
    }

});

async function createTables() {
    try {
        let file_table = "create table FILE (svg_id int auto_increment primary key, file_name varchar(60) not null, file_title varchar(256), file_description varchar(256), n_rect int not null, n_circ int not null, n_path int not null, n_group int not null, file_size int not null)";

        let exists = "select TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_NAME = 'FILE'";
        const [r, f] = await connection.execute(exists);

        if(r.length != 1) {
            await connection.execute(file_table);
            console.log("FILE table successfully created.")
        } else {
            console.log("FILE table already exists.");
        }
        
    } catch (e) {
        console.log(e);
    }
    
}

app.get('/storeFile', async function(req, res) {
    try {
        //connection = await mysql.createConnection(dbCredentials); //reconnect
        connection.open();
        if(connection) console.log("Reconnected to database");

        let insertFile = "insert into FILE values ('" + req.query.name + "','" + req.query.title + "'," + req.query.desc + "'," + req.query.rects + "'," + req.query.circs + "'," + req.query.paths + "'," + req.query.groups + "'," + req.query.size + "')";
        let fileCheck = "select from FILE where file_name='" + req.query.name + "'";
        const [r, f] = await connection.execute(fileCheck);
        if(r.length == 0) {
            try {
                await connection.execute(insertFile);
                console.log("File " + req.query.name + " successfuly saved into database.");
            } catch(e) {
                console.log("Query error: " + e);
            }
            
        } else {
            console.log("File is already stored in database");
        }

    } catch(e) {
        console.log(e);
    } finally {
        if (connection && connection.end) {
            connection.end();
            console.log("Disconnected from database");
        }
    }
});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
