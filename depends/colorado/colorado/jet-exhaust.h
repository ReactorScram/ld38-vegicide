#ifndef JETEXHAUST_H
#define JETEXHAUST_H

#include <QVector>

#include "particle-node.h"

struct JetExhaustParticle {
	float x, y, z;
	float fadeFactor;
};

class JetExhaust {
public:
	ParticleNode node;
	
	JetExhaust ();
	
	void initialize ();
	
	void step ();
	void updateVbo ();
	
	//const QVector <ParticleElement> & getParticles () const;
	
protected:
	QVector <JetExhaustParticle> particleStrings;
	QVector <ParticleElement> particles;
	
	float particleSpeed;
	
	// Length of particle strings, in particles
	int stringFactor;
	
	// Length of particle strings, 1.0 is one frame of blur
	float blurFactor;
	
	float length;
	
	float width;
	float height;
	
	float randomX () const;
	float randomZ () const;
};

#endif // JETEXHAUST_H
