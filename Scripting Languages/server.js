//
//  Exercise 9: Scripting languages
//  Part 1: Server side scripting
//  Programming Languages 2
//  Michael Beyer, Giacomo Bossi
//

//
// ! npm install express and pug !
//

var path       = require( 'path' );
var express    = require( 'express' );
var bodyParser = require( "body-parser" );
var game       = require( './game' );

var app = express();

// view engine setup
app.set( 'views', 'views' );
app.set( 'view engine', 'pug' );
app.locals.pretty = true;       // generate pretty html

var PORT = 3000;

app.use(bodyParser.urlencoded( { extended: true } ) );

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET requests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
app.get( '/', function( req, res )
{
  let gameValues = game.restart();
  res.render( 'index', gameValues );
});

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// POST requests
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
app.post( '/pl2/longestPal', function( req, res )
{
  let gameValues = game.getCurrent();

  if( req.body.hasOwnProperty( 'answer' ) )
  {
    gameValues.answerState = game.checkAnswer( req.body['answer'] );
  }

  if( req.body.hasOwnProperty( 'restart' ) )
  {
    gameValues = game.restart();
  }

  res.render( 'index', gameValues );

  game.clearAnswerState();
});

app.listen( PORT );
console.log( 'Listening on localhost, port ' + PORT );
