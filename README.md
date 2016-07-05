Smart Image
======

A small free image editing software, designed to fix damaged images. Includes adaptive filtering. It's capable of fixing such common types of damage as:
* Different kinds of noise (white noise, impulse noise).
* Low contrast.
* Low/high brightness of an image
* Blurry edges
* Shifted color balance

----

#### [User's manual](https://github.com/dive155/SmartImage/wiki)

#### [Download the lastest version](https://github.com/dive155/SmartImage/releases)

----

####Screenshot:

![](https://github.com/dive155/SmartImage/blob/master/ForGit/scr1.png)


----

#### Full list of filters:

**Adaptive:**

* Adaptive averaging filter
* Adaptive median filter
* Bilateral filter
* Dispersion-depending unsharp masking
* Local contrast-depending sharpness increase

**Simple:**

* Color correction (RGB)
* Color correction (HSL)
* Auto color balance (RGB histogram stretching)
* Auto color balance ("Gray world" method)
* Histogram stretching
* Gamma-correction
* Solarisation
* Power function transformation
* Averaging filter
* Median filter
* Gaussian blur
* Rang filter
* Average harmonic filter
* Contrharmonic filter
* Unsharp masking
* Wallace filter
* Sobel filter
* Prewitt filter
* Kirsch filter
* LoG filter (Laplacian of Gaussian)

----


#### Example:

Take a look at this picture. As you can see, it's heavily damaged:

![](https://github.com/dive155/SmartImage/blob/master/ForGit/tram_damaged.png)

Using **Smart Image**, after just a few steps you can make this picture look like this:

![](https://github.com/dive155/SmartImage/blob/master/ForGit/tram_fixed.png)

And this is what the picture used to look like before receiving damage:

![](https://github.com/dive155/SmartImage/blob/master/ForGit/tram.png)

As you can see, even though the initial image was significantly damaged, **Smart Image** was able to improve the quality of the picture to the point where the resulting image looks very similar to what it used to look like before receiving damage.
