#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/SoftShrink.c"
#else

static int nn_(SoftShrink_updateOutput)(lua_State *L)
{
  THTensor *input = (THTensor*)luaT_checkudata(L, 2, torch_Tensor);
  real lambda = luaT_getfieldchecknumber(L, 1, "lambda");
  THTensor *output = (THTensor*)luaT_getfieldcheckudata(L, 1, "output", torch_Tensor);

  THTensor_(resizeAs)(output, input);
  
  TH_TENSOR_APPLY2(real, output, real, input,				\
                   if ((*input_data) > lambda) *output_data = *input_data - lambda; \
                   else if ((*input_data) < -lambda) *output_data = *input_data + lambda; \
                   else *output_data = 0;);
  return 1;
}

static int nn_(SoftShrink_updateGradInput)(lua_State *L)
{
  THTensor *input = (THTensor*)luaT_checkudata(L, 2, torch_Tensor);
  real lambda = luaT_getfieldchecknumber(L, 1, "lambda");
  THTensor *gradOutput = (THTensor*)luaT_checkudata(L, 3, torch_Tensor);
  THTensor *gradInput = (THTensor*)luaT_getfieldcheckudata(L, 1, "gradInput", torch_Tensor);

  THTensor_(resizeAs)(gradInput, input);
  TH_TENSOR_APPLY3(real, gradInput, real, gradOutput, real, input,	\
                   if ((*input_data) > lambda || (*input_data) < -lambda) \
		     *gradInput_data = (*gradOutput_data);		\
		   else							\
		     *gradInput_data = 0;				\
    );
  return 1;
}

static const struct luaL_Reg nn_(SoftShrink__) [] = {
  {"SoftShrink_updateOutput", nn_(SoftShrink_updateOutput)},
  {"SoftShrink_updateGradInput", nn_(SoftShrink_updateGradInput)},
  {NULL, NULL}
};

static void nn_(SoftShrink_init)(lua_State *L)
{
  luaT_pushmetatable(L, torch_Tensor);
  luaT_registeratname(L, nn_(SoftShrink__), "nn");
  lua_pop(L,1);
}

#endif
