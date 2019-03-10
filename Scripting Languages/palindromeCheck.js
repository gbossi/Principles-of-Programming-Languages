//
//  Exercise 9: Scripting languages
//  Part 1: Server side scripting
//  Programming Languages 2
//  Michael Beyer, Giacomo Bossi
//

function longestPalSubstr(str)
{
    var length = str.length;
    var string = str.split('');
    let L = Array(length).fill().map(() => Array(length).fill(0));

    for(var i = 0; i<length; i++)
    {
      L[i][i]=1;
    }

    for(var cl=2; cl<length+1;cl++)
    {
      for(var i=0; i<length-cl+1;i++)
      {
        var j = i+cl-1;
        if (string[i] == string[j] && cl == 2)
          L[i][j] = 2
        else if (string[i] == string[j])
          L[i][j] = L[i + 1][j - 1] + 2
        else
          L[i][j] = Math.max(L[i][j - 1],L[i + 1][j])
      }
    }

    return (L[0][length - 1]);
}

module.exports.getLongest = longestPalSubstr;
