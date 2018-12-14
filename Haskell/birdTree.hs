--Imports
import Test.QuickCheck
import Test.QuickCheck.Function
import Data.Ratio

--Data Definition
data Tree t = Node t [Tree t] deriving Show


--Arbitraty Definition
instance Arbitrary a => Arbitrary (Tree a) where
  arbitrary =
    sized arbitrarySizedTree



--Arbitrary Specification
arbitrarySizedTree :: Arbitrary a => Int -> Gen (Tree a)
arbitrarySizedTree random = do
  node_id <- arbitrary 
  num_of_branches <- choose (0, random `div` 2)
  branches <- vectorOf num_of_branches (arbitrarySizedTree (random `div` 4))
  return (Node node_id branches)

----------------------------------------------------------------------------------------
------------------------------------THE BIRD TREE---------------------------------------
----------------------------------------------------------------------------------------

--Data Specification
bird :: Tree Rational
bird = Node 1 [left,right] where left = mapTree (^^(-1)) ((mapTree (+1) bird))
                                 right = mapTree (+1) (mapTree (^^(-1)) bird) 

--------------------------------FUNCTIONS----------------------------------------------

foldTree :: (a -> [b] -> b) -> Tree a -> b
foldTree f (Node x s) = f x (map (foldTree f) s)


sizeTree  :: Num b => Tree a -> b
sizeTree t = foldTree (\_ xs -> 1 + sum xs) t

heightTree :: (Ord b,Num b) => Tree a -> b
heightTree t = foldTree(\_ xs -> 1 + maxk xs ) t 
            where maxk xs   | xs==[] = 0 
                            | otherwise = maximum(xs) 

sumTree :: Num a => Tree a -> a
sumTree t = foldTree (\x xs -> x + sum xs) t

nodes :: Tree a -> [a]
nodes t = (foldTree (\x xs -> x: concat(xs)) t)

maxTree  :: Ord a => Tree a -> a
maxTree t = maximum (nodes t)

inTree  :: Eq a => a -> Tree a -> Bool
inTree el t =  elem el (nodes t)


countTree :: (a->Bool) -> Tree a -> Integer
countTree f t = fromIntegral (length (filter f (nodes t)))

leaves :: (Eq a) =>Tree a -> [a]
leaves t = foldTree(\x xs -> (isLeaves x xs) ++ concat xs) t 
            where isLeaves x xs | xs == [] = [x]
                                | otherwise = []

mapTree :: (a->b) -> Tree a ->Tree b
mapTree f t = foldTree(\x xs -> Node (f x) xs) t



trimTree :: Int -> Tree a -> Tree a
trimTree 0 (Node x xs) = Node x []
trimTree n (Node x xs) = Node x $ map (trimTree (n-1)) xs 


path :: [Int] -> Tree a -> a
path [] (Node x xs) = x
path p (Node x xs) = path (tail p) (xs !! head p)  



-------------------------------Properties----------------------------------------------
---------------------------------------------------------------------------------------
--Following a path of length n either in bird or in trimTree n bird we reach the same
--node.
prop_birdPath :: Int -> Property
prop_birdPath n = forAll (shuffle $ take (n-1) (cycle[0,1])) $ \x -> path x bird == (path x (trimTree n bird))  

----------------------------------------------------------------------------------------
--Following a path from the root that “zig-zags”, right-left, we pass successively all positive
--integer numbers in ascending order
prop_birdZig :: Int -> Bool
prop_birdZig n =  if n > 0 then entirePath (allToLeft(n)) bird == map realToFrac [1,2..n] 
                           else True
    where allToLeft x = take (x-1) (cycle[1,0]) 


entirePath :: [Int] -> Tree a -> [a]
entirePath [] (Node x xs) = [x]
entirePath g (Node x xs) = x:entirePath (tail g) (xs !! head g) 


----------------------------------------------------------------------------------------
--Following from the root the leftmost path, we pass successively from numbers whose 
--denominators define the Fibonacci sequence
prop_birdFibonacci :: Int -> Property 
prop_birdFibonacci limit = if (limit > 0) 
                                then forAll (elements [1,2..limit]) $ \x -> denominator (head (leaves (trimTree x (bird)))) == fib (x+1)
                                else once True

fib :: Int -> Integer
fib 0 = 0
fib 1 = 1
fib n = fib (n-1) + fib (n-2)

----------------------------------------------------------------------------------------
--Every positive rational number appears somewhere in Bird’s tree. To check this property, you
--may need to implement a function findBird q, which returns the path from the root that
--leads to the rational number q
prop_birdRatio :: Rational -> Bool
prop_birdRatio r = path (oddBird r bird) bird == r


oddBird :: Rational -> Tree Rational -> [Int]
oddBird q (Node x xs)   | q == x = []
                        | q < x  = [0] ++ evenBird q (xs!!0)
                        | q > x = [1] ++ evenBird q (xs!!1)

evenBird :: Rational -> Tree Rational -> [Int]
evenBird q (Node x xs)  | q == x = []
                        | q < x  = [1] ++ oddBird q (xs!!1)
                        | q > x = [0] ++ oddBird q (xs!!0)
----------------------------------------------------------------------------------------