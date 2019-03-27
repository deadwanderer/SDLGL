#include "../include/glm/glm.hpp"

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
        
    }
} HexCoordinates;