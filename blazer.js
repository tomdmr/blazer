/* Classes for the buttons, colors on and off */
let classes = [
    ['btn_r-off', 'btn_r-on'],
    ['btn_g-off', 'btn_g-on'],
    ['btn_b-off', 'btn_b-on'],
];
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
 * JS Zeugs um zu einem ESP an url zu reden.
 * Um default parameter erweitert.
 */
function talkToESP(url, rspFunc, errFunc){
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
function shuffle(array) {
    for (let i = array.length - 1; i > 0; i--) {
        let j = Math.floor(Math.random() * (i + 1)); // random index from 0 to i
        [array[i], array[j]] = [array[j], array[i]];
    }
}

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
