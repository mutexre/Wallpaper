//
//  OceanFFT.h
//  Glittering Wallpaper
//
//  Created by mutexre on 07/09/16.
//
//

#include <vector>
#include <Accelerate/Accelerate.h>

#pragma once

using namespace std;
using namespace glm;

class OceanFFT
{
private:
    float windSpeed;
    float windDir;
    vector<vec2> initialHeightMap;
    int patchSize;

    struct
    {
        FFTSetup setup;
        DSPSplitComplex in, out;
        vDSP_Stride stride0;
        vDSP_Stride stride1;
        vDSP_Length mlog2;
        vDSP_Length nlog2;
        int m, n;
    }
    fft;
    
    float* heightMap;

private:
    static float getRandomNumber() {
        return float(arc4random()) / float(0xffffffff);
    }
    
    static float phillips(float kx, float ky, float windSpeed, float windDirection)
    {
        float fWindDir = windDirection * M_PI / 180.0f;
        static float A = 2.f * 0.00000005f;
        float L = windSpeed * windSpeed / 9.81f;
        float w = L / 75;
        float ksqr = kx * kx + ky * ky;
        float kdotwhat = kx * cosf(fWindDir) + ky * sinf(fWindDir);
        kdotwhat = (kdotwhat > 0 ? kdotwhat : 0);
        
        float result =
                A * (pow(2.7183f, -1.0f / (L * L * ksqr)) * (kdotwhat * kdotwhat)) / (ksqr * ksqr * ksqr);
        
        float damp = (float)expf(-ksqr * w * w);
        damp = expf(-1.0 / (ksqr * L * L));
        
        result *= kdotwhat < 0.0f ? 0.25f : 1.0f;
        
        return result * damp;
    }

    void generateInitialHeightField()
    {
        initialHeightMap.clear();
        
        for (int y = 0; y < fft.n; y++)
        {
            for (int x = 0; x < fft.m; x++)
            {
                float kx = M_PI * x / float(patchSize);
                float ky = 2.0f * M_PI * y / float(patchSize);
                float Er = 2.0f * getRandomNumber() - 1.0f;
                float Ei = 2.0f * getRandomNumber() - 1.0f;
                
                float multiplier;
                if (!(kx == 0.f && ky == 0.f))
                    multiplier = sqrt(phillips(kx, ky, windSpeed, windDir));
                else
                    multiplier = 0.f;

                float amp = getRandomNumber();
                float re = multiplier * amp * Er;
                float im = multiplier * amp * Ei;
                
                initialHeightMap.push_back({ re, im });
            }
        }
    }
    
    static vec2 conjugate(const vec2& arg) {
       return vec2(arg.x, -arg.y);
    }
    
    static vec2 complex_exp(float arg) {
       return vec2(cos(arg), sin(arg));
    }
    
    void generateSpectrumKernel(float t)
    {
        for (int y = 0; y < fft.n; y++)
        {
            for (int x = 0; x < fft.m; x++)
            {
                int i = y * fft.m + x;
                
                vec2 k;
                k.x = M_PI * x / float(patchSize);
                k.y = 2.0f * M_PI * y / float(patchSize);
                
                // calculate dispersion w(k)
                float k_len = length(k);
                float w = sqrt(9.81f * k_len);
                vec2 h0_k  = initialHeightMap[i];
                vec2 h0_mk = initialHeightMap[(((fft.n - 1) - y) * fft.m) + x];
                vec2 expWT = complex_exp(w * t);
                vec2 expMWT = complex_exp(-w * t);
                vec2 h0_mk_conj = conjugate(h0_mk);
                vec2 h_tilda = vec2(h0_k.x * expWT.x - h0_k.y * expWT.y,
                                    h0_k.x * expWT.y + h0_k.y * expWT.x) +
                               vec2(h0_mk_conj.x * expMWT.x - h0_mk_conj.y * expMWT.y,
                                    h0_mk_conj.x * expMWT.y + h0_mk_conj.y * expMWT.x);
                //complex_mult(h0_k, complex_exp(w * t)) + complex_mult(conjugate(h0_mk), complex_exp(-w * t));
               
               // output frequency-space complex values
               if (x < fft.m && y < fft.n)
               {
                    fft.in.realp[i] = h_tilda.x;
                    fft.in.imagp[i] = h_tilda.y;
               }
            }
        }
    }

    void initFFT(int mlog2, int nlog2)
    {
        fft.setup = vDSP_create_fftsetup(10, kFFTRadix2);
        if (!fft.setup)
            printf("failed to create fft setup\n");
        
        fft.mlog2 = mlog2;
        fft.nlog2 = nlog2;
        fft.m = 1 << mlog2;
        fft.n = 1 << nlog2;
        
        fft.stride0 = 1;
        fft.stride1 = 0;
        
        fft.in.realp = new float[fft.m * fft.n];
        fft.in.imagp = new float[fft.m * fft.n];
        
        fft.out.realp = new float[fft.m * fft.n];
        fft.out.imagp = new float[fft.m * fft.n];
        
        patchSize = 1024;//fft.m;
    }
    
    void destroyFFT()
    {
        vDSP_destroy_fftsetup(fft.setup);
        
        if (fft.in.realp)
            delete fft.in.realp;
        
        if (fft.out.realp)
            delete fft.out.realp;

        if (fft.in.imagp)
            delete fft.in.imagp;
        
        if (fft.out.imagp)
            delete fft.out.imagp;
    }
    
    void calcHeightMap()
    {
        for (int y = 0; y < fft.n; y++)
        {
            for (int x = 0; x < fft.m; x++)
            {
                auto offset = fft.m * y + x;
                float re = fft.out.realp[offset];
                float im = fft.out.imagp[offset];
                float value = sqrt(re * re + im * im);
                
                heightMap[4 * (y * fft.m + x) + 0] = value;
                heightMap[4 * (y * fft.m + x) + 1] = value;
                heightMap[4 * (y * fft.m + x) + 2] = value;
                heightMap[4 * (y * fft.m + x) + 3] = value;
            }
        }
    }
    
    void calcRandomSpectrum()
    {
        float cx = fft.m / 2;
        float cy = fft.n / 2;
        float maxDist = sqrt(cx * cx + cy * cy);
        
        for (int y = 0; y < fft.n; y++)
        {
            for (int x = 0; x < fft.m; x++)
            {
                float rnd = 1 * float(arc4random()) / float(0xffffffff);
                
                float dx = x - cx;
                float dy = y - cy;
                float len = sqrt(dx * dx + dy * dy);
                
                float val = rnd * maxDist / (1 + len);
                
                auto offset = fft.m * y + x;
                fft.in.realp[offset] = val;
                fft.in.imagp[offset] = 0;
            }
        }
    }
    
    void performFFT()
    {
        //calcRandomSpectrum();
        //printSpectrum();
        
        vDSP_fft2d_zop(fft.setup,
                       &fft.in, fft.stride0, fft.stride1,
                       &fft.out, fft.stride0, fft.stride1,
                       fft.mlog2, fft.nlog2,
                       kFFTDirection_Inverse);

        //printHeightMap();
    }
    
    void printSpectrum()
    {
        printf("IN=\n");
        for (int y = 0; y < fft.n; y++)
        {
            for (int x = 0; x < fft.m; x++)
            {
                auto offset = fft.m * y + x;
                printf("%4.2f ", fft.in.realp[offset]);
            }
            printf("\n");
        }
        printf("\n");
    }
    
    void printHeightMap()
    {
        printf("OUT=\n");
        for (int y = 0; y < fft.n; y++)
        {
            for (int x = 0; x < fft.m; x++)
            {
                printf("%4.2f ", heightMap[4 * (y * fft.m + x) + 0]);
            }
            printf("\n");
        }
        printf("\n");
    }
    
    void printInitialHeightMap()
    {
        printf("INITIAL=\n");
        for (int y = 0; y < fft.n; y++)
        {
            for (int x = 0; x < fft.m; x++)
            {
                printf("%f ", initialHeightMap[y * fft.m + x].x);
            }
            printf("\n");
        }
        printf("\n");
    }

public:
    OceanFFT(int mlog2, int nlog2, float windSpeed, float windDir)
    {
        this->windSpeed = windSpeed;
        this->windDir = windDir;
        
        initFFT(mlog2, nlog2);
        generateInitialHeightField();
        //printInitialHeightMap();
        heightMap = new float[4 * fft.m * fft.n];
    }
    
    virtual ~OceanFFT()
    {
        destroyFFT();
    }
    
    void step()
    {
        static float t = 0;
        generateSpectrumKernel(t);
        t += 0.0000000001;
        
        performFFT();
        calcHeightMap();
    }
    
    float* getHeightMap() const {
        return heightMap;
    }
    
    int getW() const {
        return fft.m;
    }
    
    int getH() const {
        return fft.n;
    }
};