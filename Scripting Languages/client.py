#
#  Exercise 9: Scripting languages
#  Part 2: Client side scripting
#  Programming Languages 2
#  Michael Beyer, Giacomo Bossi
#

from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.options import Options

# Choose server:
#serverAddress = "https://courses.softlab.ntua.gr/pl2/2018b/exercises/palseq.php"
serverAddress = "http://localhost:3000"

playCount = 30


def longestPalSubstr(string):
    n = len(string)

    L = [[0 for x in range(n)]for y in range(n)]

    for i in range(n):
        L[i][i] = 1

    for cl in range( 2, n+1):
        for i in range(n - cl + 1):
            j = i + cl - 1
            if (string[i] == string[j] and cl == 2):
                L[i][j] = 2
            elif (string[i] == string[j]):
                L[i][j] = L[i + 1][j - 1] + 2
            else:
                L[i][j] = max(L[i][j - 1],L[i + 1][j])

    return (L[0][n - 1])


def play():
    currentRound = driver.find_element_by_xpath( "//code[@id='question']/.." )
    currentQuestion = driver.find_element_by_id( "question" )

    res = longestPalSubstr( currentQuestion.text )

    print("{0}".format( currentRound.text.replace( '\n', ', ' ) ) )
    print("Answer: {0}".format( len(currentQuestion.text) - res ) )

    form = driver.find_element_by_id( "answer" )
    form.send_keys( str(len(currentQuestion.text) - res ) + Keys.RETURN )

    print( driver.find_element_by_xpath( "//p[@class]" ).text )


# Setup
chrome_options = Options()
chrome_options.add_argument( '--headless' )
chrome_options.add_argument( '--no-sandbox' )

driver = webdriver.Chrome( chrome_options=chrome_options )
driver.get( serverAddress )

print( "Playing for {0} rounds!\n".format( playCount ) )

while playCount:
    play()

    # check for next round
    if( len( driver.find_elements_by_id( "again" ) ) != 0 ):
        driver.find_element_by_id( "again" ).click()
    else:
        print( "Could't find 'again' button!" )
        break

    playCount -= 1

print( "Exiting..." )
driver.close();
