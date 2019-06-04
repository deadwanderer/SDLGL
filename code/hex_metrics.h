#ifndef _HEX_METRICS_H
#define _HEX_METRICS_H

#include <math.h>
#include "../include/glm/glm.hpp"

enum HexDirection {
    HexDirection_NE,
    HexDirection_E,
    HexDirection_SE,
    HexDirection_SW,
    HexDirection_W,
    HexDirection_NW
}

HexDirection GetHexDirOpposite(HexDirection direction) {
    HexDirection Result;
    
    if ((uint32_t)direction < 3) {
        Result = direction + 3;
    }
    else {
        Result = direction - 3;
    }
    
    return(Result);
}

HexDirection GetHexDirPrevious(HexDirection direction) {
    HexDirection Result;
    if (direction == HexDirection_NE) {
        Result = HexDirection_NW;
    }
    else {
        Result = direction - 1;
    }
    
    return(Result);
}

HexDirection GetHexDirNext(HexDirection direction) {
    HexDirection Result;
    
    if (direction == HexDirection_NW) {
        Result = HexDirection_NE;
    }
    else {
        Result = direction + 1;
    }
    
    return(Result);
}

enum HexEdgeType {
    HexEdge_Flat,
    HexEdge_Slope,
    HexEdge_Cliff
}

static struct HexMetrics {
    // Hex chunk information
    uint32_t chunkSizeX = 5;
    uint32_t chunkSizeZ = 5;
    
    // Hex sizes
    float outerRadius = 10.0f;
    float innerRadius = outerRadius * 0.866025404f;
    
    // Hex cell solid/transition factors
    float solidFactor = 0.75f;
    float blendFactor = 1.0f - solidFactor;
    
    // elevation
    float elevationStep = 5f;
    
    // Connection terraces
    uint32_t terracesPerSlope = 2;
    uint32_t terraceSteps = terracesPerSlope * 2 + 1;
    float horizontalTerraceStepSize = 1.0f / (float)terraceSteps;
    float verticalTerraceStepSize = 1.0f / (float)(terracesPerSlope + 1);
    
    // Noise/perturbation
    // TODO(anthony): Need to add actual noise source
    // Need to figure out best format for noise (vec4s? colors? Texture2D?
    uint32_t noiseWidth;
    uint32_t noiseHeight;
    
    float cellPerturbStrength = 5.0f;
    float noiseScale = 0.3f;
    float elevationPerturbStrength = 1.5f;
    
    glm::vec3 corners[] = {
        glm::vec3(0.0f, 0.0f, outerRadius),
        glm::vec3(innerRadius, 0.0f, 0.5f * outerRadius),
        glm::vec3(innerRadius, 0.0f, -0.5f * outerRadius),
        glm::vec3(0.0f, 0.0f, -outerRadius),
        glm::vec3(-innerRadius, 0.0f, -0.5f * outerRadius),
        glm::vec3(-innerRadius, 0.0f, 0.5f * outerRadius),
        glm::vec3(0.0f, 0.0f, outerRadius)
    };
    
    glm::vec3 GetFirstCorner(HexDirection direction) {
        return corners[direction];
    }
    
    glm::vec3 GetSecondCorner(HexDirection direction) {
        return corners[direction + 1];
    }
    
    glm::vec3 GetFirstSolidCorner(HexDirection direction) {
        return corners[direction] * solidFactor;
    }
    
    glm::vec3 GetSecondSolidCorner(HexDirection direction) {
        return corners[direction + 1] * solidFactor;
    }
    
    glm::vec3 GetBridge(HexDirection direction) {
        return (corners[direction] + corners[direction + 1]) * blendFactor;
    }
    
    glm::vec3 TerraceLerp(glm::vec3 a, glm::vec3 b, uint32_t step) {
        float h = (float)step * horizontalTerraceStepSize;
        a.x += (b.x - a.x) * h;
        a.z += (b.z - a.z) * h;
        float v = (float)((step + 1) / 2) * verticalTerraceStepSize;
        a.y += (b.y - a.y) * v;
        return a;
    }
    
    glm::vec4 TerraceColorLerp(glm::vec4 a, glm::vec4 b, uint32_t step) {
        float h = (float)step * horizontalTerraceStepSize;
        a.x += (b.x - a.x) * h;
        a.y += (b.y - a.y) * h;
        a.z += (b.z - a.z) * h;
        a.w += (b.w - a.w) * h;
        return a;
    }
    
    HexEdgeType GetEdgeType(int elevation1, int elevation2) {
        if (elevation1 == elevation2) return HexEdge_Flat;
        int delta = elevation2 - elevation1;
        if (delta == 1 || delta == -1) return HexEdge_Slope;
        return HexEdge_Cliff;
    }
    
    // TODO(anthony): SampleNoise function
    glm::vec4 SampleNoise(glm::vec3 position) {
        return glm::vec4(0.0f);
    }
} HexMetrics;

int Round(float num) {
    return (num - floor(num) > 0.5f) ? ceil(num) : floor(num);
}

struct HexCoordinates {
    int x, y, z;
    
    HexCoordinates(int x, int z) {
        this.x = x;
        this.z = z;
        this.y = -x - z;
    }
    
    void FromOffsetCoordinates(HexCoordinates *hc, int x, int z) {
        hc->x = (x - z / 2);
        hc->z = z;
        hc->y = -hc->x - hc->z;
    }
    
    void FromPosition(HexCoordinates *hc, glm::vec3 position) {
        float x =  position.x / (HexMetrics.innerRadius * 2);
        float y = -x;
        float offset = position.z / (HexMetrics.outerRadius * 3f);
        x -= offset;
        y -= offset;
        int iX = Round(x);
        int iY = Round(y);
        int iZ = Round(-x - y);
        
        if (iX + iY + iZ != 0) {
            float dX = (float)fabs(x - (float)iX);
            float dY = (float)fabs(y - (float)iY);
            float dZ = (float)fabs(-x - y - (float)iZ);
            
            if (dX > dY && dX > dZ) {
                iX = -iY - iZ;
            }
            else if (dZ > dY) {
                iZ = -iX - iY;
            }
        }
        
        hc->x = iX;
        hc->z = iZ;
        hc->y = -hc->x - hc->z;
    }
    
    
} HexCoordinates;

#endif