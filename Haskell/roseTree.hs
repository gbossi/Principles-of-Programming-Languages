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
--The height of every tree (heightTree) is a positive integer number that does not exceed the
--size of the tree (sizeTree).
prop_correctHeight :: (Eq a) => Tree a -> Bool
prop_correctHeight tree = (heightTree tree > 0) && (heightTree tree <= sizeTree tree)

----------------------------------------------------------------------------------------
--The maximum value of a tree (maxTree) appears in the tree (inTree).
prop_correctMax :: (Ord a) => Tree a -> Bool
prop_correctMax tree = inTree(maxTree tree) tree

----------------------------------------------------------------------------------------
--All the nodes (nodes) of a tree appear in the tree.
prop_correctNodes :: (Eq a,Show a) => Tree a -> Property
prop_correctNodes tree = forAll (elements (nodes tree)) $ \x -> inTree x tree

----------------------------------------------------------------------------------------
--The number of nodes in a tree that satisfy a predicate (countTree) is a natural number that
--does not exceed the size of the tree.
prop_correctCount :: Fun a Bool -> Tree a -> Bool
prop_correctCount (Fun _ f) tree = ((countTree f tree) <= (sizeTree tree)) && ((countTree f tree) >=0)

----------------------------------------------------------------------------------------
--The number of nodes of a tree is equal to its size. Also, the number of leaves is less than the
--number of nodes, unless they are both equal to 1.
prop_correctDimension ::  (Eq a) => Tree a -> Bool
prop_correctDimension tree = (length (nodes tree) == sizeTree tree) && ((length (leaves tree) <= sizeTree tree) || ((length (leaves tree)) == 1 && (sizeTree tree ==1)))

----------------------------------------------------------------------------------------
--The function mapTree preserves the size and the height of trees.
prop_correctMap :: Fun a Bool -> Tree a -> Bool
prop_correctMap (Fun _ f) tree = (sizeTree (mapTree f tree)== sizeTree tree) && (heightTree (mapTree f tree)== heightTree tree)

----------------------------------------------------------------------------------------
--If the value n appears in tree t, then the value f n apperas in tree mapTree f t.
prop_correctAppMap :: (Eq a,Show a) => Fun a Bool -> Tree a -> Property
prop_correctAppMap (Fun _ f) tree = forAll (elements (nodes tree)) $ \x -> inTree (f x) (mapTree f tree)

----------------------------------------------------------------------------------------
--For all functions g 2 f nodes, leaves g the following holds: map f . g == g . mapTree f.
prop_correctGoodCombination ::  (Eq a) =>  Fun a Bool -> Tree a -> Bool
prop_correctGoodCombination (Fun _ f) tree =  (map f.nodes) tree == (nodes.mapTree f) tree && (map f.leaves) tree == (leaves.mapTree f) tree



