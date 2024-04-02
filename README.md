### Rainfall

Random tool I made to start learning c++

Basically you pass it some file containing some ascii art (ex. file `city.cat` included), and it creates a rainfall effect over the image in your terminal

Problems:
 - It doesn't look great because you'll see your term cursor jumping around as it's repainting. It's much better than it was originally though, I got rid of the flicker by not clearing term and instead just painting over the prev frame
 - The handrolled RNG has a distribution problem, there are big "clumps" of raindrops towards the beginning of each line
 - The first few chars in each line never get a raindrop, another problem with the RNG
   - I did the handrolled RNG thinking "maybe this could be used somewhere that MT/etc. isn't possible"
 - Need to gracefully handle exiting & not rely on OS to reclaim manual allocations
 - Probably other bugs
