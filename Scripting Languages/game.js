//
//  Exercise 9: Scripting languages
//  Part 1: Server side scripting
//  Programming Languages 2
//  Michael Beyer, Giacomo Bossi
//

var pal = require( './palindromeCheck' );

var currentLevel = 0;
var answerState  = '';

var questions = [ 'baaab',
                  'aaaaa',
                  'abbab',
                  'bbabba',
                  'abbaab',
                  'bbcacbcbb',
                  'bbccacac',
                  'cccaccbbba',
                  'babaacabca',
                  'dabdbcccdbbcdddbbdda',
                  'ccadabbaaacbbacbccdd',
                  'rl(la1u048xyk6(72dix',
                  'qdrlnbuegst5nh95nroz',
                  '00n9jyq094ai5j3w0my2',
                  'z4em)cxykd3dcwym(cv(',
                  'ccgddgfbfgccecffgbbfggdgdeegceefaebddafcgaeeccccdeaddedaba' +
                  'gefdcfaebefg',

                  'gaecdhggdchhgecggdfebbhhdebhdbhbcdegccfgfefdbacadaefceffbh' +
                  'feaefdabccdhabegfgghgcadachfhaefefbcacddf',

                  '5s7pownvoyhu0(a(tgte3)0lsiqdb4863quatk9r65j0usv77v2fjmrxec' +
                  'ul9k1o7dcnvscyesmd5ni95db6aesp46jmjcobxi8',

                  '8h9oh25xiafxvo(yjc04csamqat7ugq4gd3oezf1cv5h(8gwq5ppnbj9tp' +
                  'snpf27zz8l0u)5pypdpirvo)xl2dqx(5xhc13eoga3',

                  'cgahegfdgegghgagdahigdfifgbaahadfacbhifedccbaciecgdibiigfb' +
                  'hgihbfidghcccfffhghhbbffbgeadbbcibaagaeehhhbeegdbebdggihde' +
                  'ihgbbfcbgaccfaaebeihiadadbhdachiaeabaddhegaagafifehffcfadd' +
                  'ddfbdgfdighdecafdhgig',

                  'a5s3w4l48x7gzj)vs1bmky0ds3hdmx8q3h)s0t4e391wwt)qtxdmb9yn2f' +
                  'h9z1ebf28iw3qei5(rsq(l0icvh2kwmq9a25qwrx)5znnme9dl2wrjc9bt' +
                  'ujchennt5d(w7zrt7m7g49vvpy4bedg00bh58837)(kwe)afo1(z2rxmye' +
                  'esmkga1aqr5dspi32gnw(88dejle1o9nnp8h(1bbnzhp2)3ql2zwo(iu1n' +
                  'cqhum60tm)td81dtwt5yjiw2u84jf26whkdco7sffn7a3vl74ic2e7dm8(' +
                  '1wfbn30ijs3w81nuy0zdxzvbshrc1(1l7edipsn68ohjuzsieio7frkr0b' +
                  'cdvzgtdmsjfv9r1tmoal98k145geplx7sn4jf3pje7(ybyhuzr05ya3gq7' +
                  'w3(heceyaxj5jpw071vx1xydedn6b00r1jgsiy1cl9qd(nzq7h2kwzx4(i' +
                  'ajz6fmx2q1whc6h8lmgcm3wbnhz9kffamwvp1dl2sjzrsl07ioa58twyjj' +
                  'tpqly4l6r76fqowgxdb4(wbad7zyfmfd0)oa057e4wrj2tsjf5z022ygkt' +
                  'htc5en1p4lw3pkh2pkhgxvm4546ravgjxnbqb57o4mipeebs252j351wr4' +
                  'kidze91x098gsa1u(dpw)bfzu2cardi2b24)(fw17d6l9ky1yfypaxdm49' +
                  '69pzorhv5b8fboz)1qd64xl4w)7b75fccbxaqut)2c0qdzq08zdenhadxc' +
                  'wg0rqj06e3b7st)vzwxcubmz7p7bvvjggw0doxbuek9)mmc4l(2af2xab2' +
                  'e9y1dk78l((z90kpjesggqtxym2cjuyvqiw70cpova1u2zz()v3)snicdd' +
                  'onkstodiwx44ujiqfamlxr7j(9(8ki561sfbju)oes2tcsx36wxui50rmd' +
                  'xw4owdivyxiauzzgm9qplrkaqbibtngfao16(xmvwnjdspatu4j512ala2' +
                  'g41knr6pi85xyi'
                ];

function getCurrent()
{
  if( currentLevel > questions.length - 1 )
    currentLevel = questions.length - 1;

  return {
           questionNr: currentLevel + 1,
           questionLength: questions[currentLevel].length,
           question: questions[currentLevel],
           answerState: answerState
         };
}

function checkAnswer( answer )
{
  let currentPal = questions[currentLevel];
  let res = pal.getLongest( currentPal );

  if( ( currentPal.length - answer == res) && answer != '' )
  {
    currentLevel++;

    answerState = currentLevel > questions.length - 1 ? 'restart' : 'correct';
  }
  else
  {
    answerState = 'wrong';
  }

  return answerState;
}

function clearAnswerState()
{
  answerState = '';
}

function restart()
{
  console.log( "Restarting game..." );
  currentLevel = 0;
  answerState = '';
  return getCurrent();
}

module.exports.getCurrent       = getCurrent;
module.exports.checkAnswer      = checkAnswer;
module.exports.clearAnswerState = clearAnswerState;
module.exports.restart          = restart;
