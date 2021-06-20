/* Classes for the buttons, colors on and off */
let classes = [
    ['btn_r-off', 'btn_r-on'],
    ['btn_g-off', 'btn_g-on'],
    ['btn_b-off', 'btn_b-on'],
];
/* Some cookie-functions for persistent storage. This is not the
 * world's best solution, as many people, including me, usually drop all
 * cookies when they quit the browser. With mobile devices in mind, I do not quit
 * the browsers too often. But yes, cookies are not the final word
 *
 * returns the cookie with the given name,
 * or undefined if not found
 */
/*
function getCookie(name) {
    let matches = document.cookie.match(new RegExp(
        '(?:^|; )' + name.replace(/([\.$?*|{}\(\)\[\]\\\/\+^])/g, '\\$1') + '=([^;]*)'
    ));
    return matches ? decodeURIComponent(matches[1]) : undefined;
}
*/
/*
 * remove all cookies associated to local storage.
 *
 */
/*
function removeCookies() {
    let res = document.cookie;
    if( res === '') return;
    let multiple = res.split(';');
    for(let i = 0; i < multiple.length; i++) {
        let key = multiple[i].split('=');
        document.cookie = key[0]+' =; expires = Thu, 01 Jan 1970 00:00:00 UTC';
    }
}
*/
/* boiler plate */
let dict = [];
let messageChain=function(v, b){ return; }
function initWebSocket(gw){
    //console.log('Trying to open '+gw);
    ws = new WebSocket(gw);
    ws.onopen    = onOpen;
    ws.onclose   = onClose;
    ws.onmessage = onMessage;
    //console.log(websocket);
    return ws;
}
function onOpen(event){
    event.target.send('STATE');
}
function onClose(event){
    console.log((new Date(Date.now())).toISOString()+': Connection close');
    //console.log(event);
    //console.log(event.explicitOriginalTarget.url);
    //console.log(event.target.url);
    setTimeout(initWebSocket, 2000, event.explicitOriginalTarget.url);
}
function onError(event){
    console.log('Error:');
    console.log(event);
}

/*
 **
 ** Next version: Do the parsing, plug all values into "b" and then chain a handler
 **
 */
function onMessage(event){
    //console.log('Got event');
    //console.log(event);
    //console.log(event.target.myId);
    let b = dict[ event.target.myId ];
    let payload = event.data;
    //console.log(payload);
    if( m = payload.match(/([A-Z]+)(\d*) (\d*) (\d*)/)){
        // m[2]: State of LEDs
        // m[3]: millis() since start
        // m[4]: Read out of Touchsensor
        //console.log(m);
        b.delta  = m[3] - b.millis;
        b.millis = m[3];
        b.touch  = m[4];
        if( m[1] === 'STATE'){
            for(let i=0; i<3; i++){
                b.state[i] = parseInt(m[2][i],10);
                b.btn[i].setAttribute('class', 'button '+classes[i][b.state[i]]);
            }
        }
        messageChain(m[1], b);
    }
    else if(payload.startsWith('CLOSE')){
        //console.log('Device goes to sleep');
        event.target.close();
    }
    else{
        console.log('Unknown: '+payload);
    }
}
/* -- unused
function doState(){
    websocket.send('STATE');
}
*/
function createLEDTable(tableId, urlParams){
    let table = document.getElementById(tableId);
    let row  = table.insertRow(-1);
    let cell = row.insertCell(0);
    for(let i=0; i<3; i++){
        cell = row.insertCell(-1);
        let input = document.createElement('input');
        input.type = 'checkbox';
        input.setAttribute('id', 'C' + i);
        input.setAttribute('checked', 'true');
        cell.appendChild(input);
    }

    let i = 0;
    urlParams.forEach(function(item){
        let btn = [];
        // row at the end
        row = table.insertRow(-1);
        cell = row.insertCell(0);
        cell.innerHTML = item;
        cellr = row.insertCell(-1);
        cellr.setAttribute('id', 'D'+i+'0');
        cellr.setAttribute('class', 'button btn_r-off');
        cellr.setAttribute('onclick', 'dynamic('+i+',0)');
        cellr.innerHTML = '&nbsp;';

        cellg = row.insertCell(-1);
        cellg.setAttribute('id', 'D'+i+'1');
        cellg.setAttribute('class', 'button btn_g-off');
        cellg.setAttribute('onclick', 'dynamic('+i+',1)');
        cellg.innerHTML = '&nbsp;';

        cellb = row.insertCell(-1);
        cellb.setAttribute('id', 'D'+i+'2');
        cellb.setAttribute('class', 'button btn_b-off');
        cellb.setAttribute('onclick', 'dynamic('+i+',2)');
        cellb.innerHTML = '&nbsp;';


        btn = [ cellr, cellg, cellb ];
        //console.log('Start WS');
        let webs = initWebSocket('ws://'+item+'/ws');
        console.log(i);
        webs.myId = i;
        i += 1;
        dict.push({
            btn: btn,
            state: [0,0,0],
            touch: 0,
            delta: 0,
            millis: 0,
            url: item,
            webs: webs
        });
    });
}

/*
 *  Aux function to shuffle the elements of an array.
 */
function shuffle(array) {
    for (let i = array.length - 1; i > 0; i--) {
        let j = Math.floor(Math.random() * (i + 1)); // random index from 0 to i
        [array[i], array[j]] = [array[j], array[i]];
    }
}
function Sleep(milliseconds) {
    return new Promise(resolve => setTimeout(resolve, milliseconds));
}
function launch(url){
    //console.log(url);
    let table = document.getElementById("BlazerTable");
    let param="";
    let es = 0;
    for (let i = 0, row; row = table.rows[i]; i++) {
        let n = row.cells[1].firstChild.value;
        let cb = row.cells[0].getElementsByTagName('input')[0];
        if(cb.checked){
            if( param === "") param += '?';
            else      param += '&';
            param += es+'='+ n;
            es++;
        }
    }
    //console.log(url+param);
    window.open(url+param, "_top");
}
/*
function ping(host, port, pong, id) {
    var started = new Date().getTime();

    var http = new XMLHttpRequest();

    http.open("GET", "http://" + host + ":" + port, true); // async=true
    http.timeout = 5000;
    http.error = function(event){
        console.log(event);
    };
    http.onreadystatechange = function() {
        if (http.readyState == 4) {
            var ended = new Date().getTime();
            var milliseconds = ended - started;
            if (pong != null) {
                pong(host, milliseconds, id);
            }
        }
        else{ console.log('ReadyState: '+ http.readyState); }
    };
    try {
        http.send(null);
    } catch(exception) {
        console.log("Exception");
    }
}

function doPong(host, pong){
    console.log('Response from '+host+': '+pong+ 'ms');
    if (Number(pong) < 1800){
        console.log('Is alive');
    }
    else{
        console.log('Is dead');
    }
}
function pingpong(cell){
    let row=cell.parentNode;
    host = row.cells[1].firstChild.value
    console.log(host);
    let cb = row.cells[0].getElementsByTagName('input')[0];
    ping(host, 80, doPong, cb)
}
*/
