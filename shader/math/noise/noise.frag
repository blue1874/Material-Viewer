float fade(float x, float y, float t)
{
    float ft = t * t * t * (t * (t * 6 - 15) + 10);
    return x + (y - x) * ft;
}

vec2 hash22(vec2 p)
{
    p = vec2( dot(p,vec2(127.1,311.7)),
              dot(p,vec2(269.5,183.3)));

    return -1.0 + 2.0 * fract(sin(p)*43758.5453123);
}

float perlin_noise_2d(vec2 p)
{
    vec2 pi = floor(p);
    vec2 pf = p - pi;

    // linear
    // vec2 w = pf;

    // fade 
    vec2 w = pf * pf * pf * (pf * (pf * 6 - 15) + 10);

    // classic
    // vec2 w = pf * pf * (3.0 - 2.0 * pf);

    return mix(mix(dot(hash22(pi + vec2(0.0, 0.0)), pf - vec2(0.0, 0.0)), 
                   dot(hash22(pi + vec2(1.0, 0.0)), pf - vec2(1.0, 0.0)), w.x), 
               mix(dot(hash22(pi + vec2(0.0, 1.0)), pf - vec2(0.0, 1.0)), 
                   dot(hash22(pi + vec2(1.0, 1.0)), pf - vec2(1.0, 1.0)), w.x),
               w.y);
}

// Skewing and unskewing factors for 2, 3, and 4 dimensions
float F2 = 0.5f *(sqrt(3.0) - 1.0);
float G2 = (3.0-sqrt(3.0))/6.0;
float F3 = 1.0f / 3.0f;
float G3 = 1.0/6.0;
float F4 = (sqrt(5.0)-1.0)/4.0;
float G4 = (5.0-sqrt(5.0))/20.0;

float simplex_noise_2d(vec2 p) 
{
    float n0, n1, n2; // Noise contributions from the three corners
    // Skew the input space to determine which simplex cell we're in
    float s = (p.x + p.y) * F2; // Hairy factor for 2D
    vec2 pi = floor(p + vec2(s));
    float t = (pi.x + pi.y) * G2;
    vec2 pf = p - (pi - vec2(t));
    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    vec2 dp;
    if(pf.x > pf.y) {dp.x=1; dp.y=0;} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
    else {dp.x=0; dp.y=1;}      // upper triangle, YX order: (0,0)->(0,1)->(1,1)
    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6
    vec2 pf1 = pf - dp + vec2(G2);
    vec2 pf2 = pf - vec2(1.0 - 2.0 * G2);
    // Work out the hashed gradient indices of the three simplex corners
    // Calculate the contribution from the three corners
    float t0 = 0.5 - pf.x * pf.x - pf.y * pf.y;
    if(t0<0) n0 = 0.0;
    else {
      t0 *= t0;
      n0 = t0 * t0 * dot(hash22(pi), pf);  // (x,y) of grad3 used for 2D gradient
    }
    float t1 = 0.5 - pf1.x * pf1.x - pf1.y * pf1.y;
    if(t1<0) n1 = 0.0;
    else {
      t1 *= t1;
      n1 = t1 * t1 * dot(hash22(pi + dp), pf1);
    }
    float t2 = 0.5 - pf2.x * pf2.x -pf2.y * pf2.y;
    if(t2<0) n2 = 0.0;
    else {
      t2 *= t2;
      n2 = t2 * t2 * dot(hash22(pi + vec2(1.0)), pf2);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 70.0 * (n0 + n1 + n2);
}

const int frequency = 4;
float FBM_noise_2d(vec2 p)
{
    float ans = 0.0f;
    for(int i = 0; i < frequency; i++)
    {
        ans += simplex_noise_2d(p * (1 << i)) / (1 << i);
    }
    return ans;
}

float randomNoise(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}