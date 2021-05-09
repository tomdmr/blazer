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
 */
/*
 * returns the cookie with the given name,
 * or undefined if not found
 */
function getCookie(name) {
    let matches = document.cookie.match(new RegExp(
        "(?:^|; )" + name.replace(/([\.$?*|{}\(\)\[\]\\\/\+^])/g, '\\$1') + "=([^;]*)"
    ));
    return matches ? decodeURIComponent(matches[1]) : undefined;
}
/*
 * remove all cookies associated to local storage.
 *
 */
function removeCookies() {
    let res = document.cookie;
    if( res === "") return;
    let multiple = res.split(";");
    for(let i = 0; i < multiple.length; i++) {
        let key = multiple[i].split("=");
        document.cookie = key[0]+" =; expires = Thu, 01 Jan 1970 00:00:00 UTC";
    }
}

/* boiler plate */
let dict = [];
function initWebSocket(gw){
    console.log('Trying to open '+gw);
    websocket = new WebSocket(gw);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
    console.log(websocket);
    return websocket;
}
function onOpen(event){
    event.target.send('STATE');
}
function onClose(event){
    console.log((new Date(Date.now())).toISOString()+': Connection close');
    console.log(event);
    //console.log(event.explicitOriginalTarget.url);
    console.log(event.target.url);
    setTimeout(initWebSocket, 2000, event.explicitOriginalTarget.url);
}
function onError(event){
    console.log('Error:');
    console.log(event);
}

function onMessage(event){
    //console.log('Got event');
    //console.log(event);
    //console.log(event.target.myId);
    let b = dict[ event.target.myId ];
    //console.log(b);
    let payload = event.data;
    //console.log(payload);
    if (payload.startsWith("STATE")){
        console.log("State responded");
        b.state[0] = parseInt(payload[5],10);
        b.state[1] = parseInt(payload[6],10);
        b.state[2] = parseInt(payload[7],10);
        b.btn[0].setAttribute('class', 'button '+classes[0][b.state[0]]);
        b.btn[1].setAttribute('class', 'button '+classes[1][b.state[1]]);
        b.btn[2].setAttribute('class', 'button '+classes[2][b.state[2]]);
        //console.log(b.state);
    }
    else if(payload.startsWith("TOUCH")){
        console.log("We got the touch");
        /* 4th button is currently missing
        if(b.btn[3].innerHTML === 'Touch' ){
            b.btn[3].innerHTML = 'Yeah!';
            setTimeout(function reset(){b.btn[3].innerHTML = 'Touch';}, 100);
        }
        */ 
    }
    else if(payload.startsWith("CLOSE")){
        console.log("Device goes to sleep");
        event.target.close();
    }
}

function doState(){
    websocket.send("STATE");
}


function createLEDTable(tableId, gateways){
    // Make table
    table = document.getElementById(tableId);
    gateways.forEach(function(item, itemIdx){
        let btn = [];
        // raw at the end
        let row = table.insertRow(-1);
        let cell = row.insertCell(0);
        cell.innerHTML = 'Blaze '+itemIdx+'<br>'+item;

        cellr = row.insertCell(-1);
        cellr.setAttribute("id", 'D'+itemIdx+'0');
        cellr.setAttribute("class", "button btn_r-off");
        cellr.setAttribute("onclick", "btnLEDClick("+itemIdx+",0)");
        cellr.innerHTML = '';

        cellg = row.insertCell(-1);
        cellg.setAttribute("id", 'D'+itemIdx+'1');
        cellg.setAttribute("class", "button btn_g-off");
        cellg.setAttribute("onclick", "btnLEDClick("+itemIdx+",1)");
        cellg.innerHTML = '';

        cellb = row.insertCell(-1);
        cellb.setAttribute("id", 'D'+itemIdx+'2');
        cellb.setAttribute("class", "button btn_b-off");
        cellb.setAttribute("onclick", "btnLEDClick("+itemIdx+",2)");
        cellb.innerHTML = '';

        btn = [ cellr, cellg, cellb ];
        let webs = initWebSocket(item);
        webs.myId = itemIdx;
        dict.push({
            btn: btn,
            state: [0,0,0],
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
/* To here */
