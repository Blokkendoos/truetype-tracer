#pragma once

#include "writer.hpp"

class NGC_Writer : public Writer {
public:
    NGC_Writer( std::string font, std::string text, bool unicode, double scale, bool blockdelete = false ) 
        :  ttfont(font), text(text), unicode(unicode), scale(scale), bd(blockdelete) {}
    
    virtual bool has_conic() {return true;}
    virtual bool has_cubic() {return true;}
    
    virtual void preamble() {
        std::cout << "(font: "<< ttfont <<")\n"; //,ttfont);
        
        if(!unicode) {
            std::cout << "(text: ";
            const char* s = text.data();
            int l = text.length();
            for (int i=0; i<l; i++)
                if(isalnum(s[i]))
                    std::cout << s[i];
                else
                    std::cout << "*";
            std::cout << ")\n";
        }
        
        std::cout << "#1=0.1    (SafeHeight)\n";
        std::cout << "#2=0.01   (Depth of Cut)\n";
        std::cout << "#3="<< scale << " (XY Scale)\n"; //, scale); // scale used here!
        std::cout << "#4=10.0   (Feed)\n";
        std::cout << "#5=0.0    (X offset)\n";
        std::cout << "#6=0.0    (Y offset)\n";
    
        //std::cout << " (g-code preamble)\n";
    }
    virtual void postamble(long int offset, extents line_extents) {
        std::cout << "(final X offset: "<< offset << ")\n"; //, offset);
        if ( line_extents.maxx > line_extents.minx ) {
            std::cout << "(overall extents: X = "<< line_extents.minx <<" to ";
            std::cout << line_extents.maxx << ", Y = " << line_extents.miny << "  to ";
            std::cout << line_extents.maxy << ")\n"; //, line_extents.minx, , line_extents.miny, line_extents.maxy);
        }
        std::cout << "G00 Z #1\nM02\n";
    }
    virtual void start_glyph(const char* s,wchar_t wc, long int offset) {
        if(isalnum(*s))
            std::cout << "(start of symbol " << (char)wc << ")\n"; //, wc);
        else
            std::cout << "(start of symbol 0x"<< std::hex << wc << std::dec <<")\n"; //, wc);
        
        /* comment with offset info */
        std::cout << "(starting X offset: "<< offset << ")\n"; //, offset);
    }
    virtual void end_glyph(extents glyph_extents, FT_Vector advance) {
        if ( glyph_extents.maxx > glyph_extents.minx ) {
            std::cout << "(symbol extents: X = " << glyph_extents.minx;
            std::cout << " to " << glyph_extents.maxx;
            std::cout << ", Y = " << glyph_extents.miny;
            std::cout << " to " << glyph_extents.maxy << ")\n";
        }
        std::cout << "(symbol advance: X = "<< advance.x << ", Y = " << advance.y <<")\n"; //, ,  );
    }
    virtual void move_to(P p) {
        //if (debug) printf("(moveto %ld,%ld)\n", to->x, to->y);
        //char *blockdelete = user? "/": "";
        //printf("%sG00 Z #1\n", blockdelete);
        //printf("%sG00 X [%ld*#3+#5] Y [%ld*#3+#6] (moveto)\n", blockdelete, to->x, to->y);
        //printf("%sG01 Z [0-#2] F#4\n", blockdelete);
        std::cout << "G00 Z #1\n";
        std::cout << "G00 X [" << p.x << "*#3+#5] Y [" << p.y << "*#3+#6] (moveto)\n";
        std::cout << "G01 Z [0-#2] F#4\n"; //, blockdelete);
    }
    virtual void line(P p) {
        std::cout << "G01 X [" << p.x << "*#3+#5] Y [" << p.y << "*#3+#6] (lineto)\n";
    }
    virtual void line_to(P p) {
        //char *blockdelete = user? "/": "";
        // printf("%sG01 X [%ld*#3+#5] Y [%ld*#3+#6] (lineto)\n", blockdelete, to->x, to->y);
        std::cout << "G01 X [" << p.x << "*#3+#5] Y [" << p.y << "*#3+#6] (lineto)\n"; //, blockdelete, to->x, to->y);
    }
    virtual void conic_to(P to, P diff ) {
        std::cout << "G5.1 X[" << to.x << "*#3+#5] Y[" << to.y << "*#3+#6] I["<< diff.x <<"*#3] J[" << diff.y <<"*#3]\n";
    }
    virtual void cubic_to(P ctrl1, P ctrl2, P to ) {
        std::cout << "G5.2 X[" << ctrl1.x << "*#3+#5] Y[" << ctrl1.y << "*#3+#6] L4 P1\n";
        std::cout << "X[" << ctrl2.x << "*#3+#5] Y[" << ctrl2.y << "*#3+#6] P1\n"; //, control2->x, control2->y
        std::cout << "X[" << to.x << "*#3+#5] Y[" << to.y << "*#3+#6] P1\n";
        std::cout << "G5.3\n";
    }
    virtual void arc_small_den(P p2) {
        std::cout << "G1 X["<< p2.x <<"*#3+#5] Y["<< p2.y<<"%.4f*#3+#6]\n"; //, p2.x, p2.y);
    }
    virtual void arc(P p2, double r, double gr, double bulge) {
        if(r < 0)
            std::cout << "G3 X[" << p2.x << "*#3+#5] Y["<< p2.y  << "*#3+#6] R["<< gr <<"*#3]\n";
        else
            std::cout << "G2 X[" << p2.x << "*#3+#5] Y["<< p2.y  << "*#3+#6] R["<< gr <<"*#3]\n";

    } 
private:
    
    double scale;
    bool bd; // blockdelete
    std::string ttfont;
    std::string text;
    bool unicode;
    std::ostringstream stream;
};
