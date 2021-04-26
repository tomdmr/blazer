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
/*
 * Function to talk to an ESP. Parameters:
 * url: Required. what you want to send to the ESP, like "http://192.168.178.55/set?0=1"
 * rspFunction(data): Optional, what to do with the responded text. Default: log to console
 * errFunction(err) : Optional, what to do if the fetch request failed. See index.html for an example.
 *                    Default:  Send error to console
 */
function talkToESP(url, rspFunc, errFunc){
    console.log('talkToESP: '+url);
    rspFunc = rspFunc || function(data){
        console.log(data);
    };
    errFunc = errFunc || function(err){
        console.log('talkToESP: Ooops: '+err);
    }
    fetch(url)
        .then(function(response){
            if (response.status !== 200) {
                console.log('Looks like there was a problem. Status Code: ' +
                            response.status);
                return;
            }
            // Check for response
            response.text().then(rspFunc);
        })
        .catch(errFunc);
}
/*
 * Create a string that represensts the inner content of the status table. It reads the array of
 * contact addresses and creates a line in a table for each.
 * The function returns the HTML string
 */
function makeStatusTable(urlAddr){
    var result='';
    urlAddr.forEach(function(item, itemIdx){
        result += '<tr><td>Blaze '+itemIdx+'</td>\n';
        result += '<td class="button btn_r-off" id="R' + itemIdx + '0" onclick="btnImm(this,'+itemIdx+',0)">Aus</td>\n'+
                  '<td class="button btn_g-off" id="R' + itemIdx + '1" onclick="btnImm(this,'+itemIdx+',1)">Aus</td>\n'+
                  '<td class="button btn_b-off" id="R' + itemIdx + '2" onclick="btnImm(this,'+itemIdx+',2)">Aus</td>\n'+
                  '</tr>\n';
        states.push(['0', '0', '0']);
    });
    return result;
}
/*
 * Convenience function. Toggle the state of one LED. row is the
 * Blazer number, col goes from 0 to 2
*/
function btnImm(btn, row, col){
    if(states[row][col] === '0' ){
        talkToESP(ipAddr[row]+'set?'+col+'=1');
        states[row][col] = '1';
        btn.innerHTML = 'An';
        btn.setAttribute('class', 'button '+classes[col][1]);
    }
    else{
        talkToESP(ipAddr[row]+'set?'+col+'=0');
        states[row][col] = '0';
        btn.innerHTML = 'Aus';
        btn.setAttribute('class', 'button '+classes[col][0]);
    }
}

function btnImm2(row, col, state){
    btn = document.getElementById('R'+row+col);
    if( state > 0){
        talkToESP(ipAddr[row]+'set?'+col+'=1');
        states[row][col] = '1';
        btn.innerHTML = 'An';
        btn.setAttribute('class', 'button '+classes[col][1]);
    }
    else{
        talkToESP(ipAddr[row]+'set?'+col+'=0');
        states[row][col] = '0';
        btn.innerHTML = 'Aus';
        btn.setAttribute('class', 'button '+classes[col][0]);
    }
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
/*
 * Fingerübung für getimten Phasenablauf. Funktioniert nicht.
 * Das funktionierende Sample ist in sample02.html
*/
function phasing(phases){
    let i = 0;
    func( phases[i++] );
    setTimeout(function run() {
        // Change state
        func(i++);
        setTimeout(run, 100);
    }, 100);
    /*
       phases.forEach(function(phase){
       let timerId = setTimeout(function tick(){
       });
       });
     */

}
/* Getting my parameters: */
/*
   const queryString = window.location.search;
   const urlParams = new URLSearchParams(queryString);
   if(urlParams.has('whatever')){ doSomething(); }
 */
