/* Copyright 2016 - Derek Kwan
 * Distributed under GPL v3 */

#include <math.h>
#include "m_pd.h"

static t_class *dklrnd_class;

typedef struct _dklrnd {
	t_object x_obj;
	t_float x_f;
	unsigned int x_state;
	t_float x_minmax; //0 = tend towards 0, nonzero = tend towards high
} t_dklrnd;

static int linseed(void){
	static unsigned int linrandseed = 1997333137;
	linrandseed = linrandseed * 2891336453 + 1500450271;
	linrandseed = linrandseed % 4294967296;
	return (linrandseed & 0x7fffffff);
}

static void *dklrnd_new(t_floatarg f, t_floatarg minmax){
	t_dklrnd *x = (t_dklrnd *)pd_new(dklrnd_class);
	x->x_f = f;
	x->x_minmax = minmax;
	x->x_state = linseed();
	floatinlet_new(&x->x_obj, &x->x_f);
	floatinlet_new(&x->x_obj, &x->x_minmax);
	outlet_new(&x->x_obj, &s_float);
	return (x);
}

static void dklrnd_bang(t_dklrnd *x){
	int n = x->x_f;
	float lrmm = x->x_minmax;
	float nval;
	int range = (n < 1 ? 1 : n);
	int lohi;
	if(lrmm <= 0.){
		lohi = 0;
		}
	else{
		lohi = 1; // > 0
	};
	unsigned int randval = x->x_state;
	x->x_state = randval = ((randval * 2891336453 + 1500450271) % 4294967296) & 0x7fffffff;
	if(lohi == 0){ //tend towards 0
		nval = (float)range*(1.-pow(1.-(randval * (1./2147483647.)),0.5)); //divide by 2^31
		}
	else{ //tend towards 1
		nval = (float)range*(pow(randval * (1./2147483647.), 0.5));
		};
	if(nval >= range){
		nval = range - 1.;
		};
	outlet_float(x->x_obj.ob_outlet, nval);
}

static void dklrnd_seed(t_dklrnd *x, t_float f, t_float glob){
	x->x_state = (int)f;
}

void dklrnd_setup(void){
	dklrnd_class = class_new(gensym("dklrnd"), (t_newmethod)dklrnd_new, 0,
			sizeof(t_dklrnd), 0, A_DEFFLOAT, A_DEFFLOAT, 0);
	class_addbang(dklrnd_class, dklrnd_bang);
	class_addmethod(dklrnd_class, (t_method)dklrnd_seed, gensym("seed"), A_FLOAT, 0);
}
