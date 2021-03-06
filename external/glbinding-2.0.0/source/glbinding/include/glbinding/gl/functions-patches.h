
#pragma once

#include <glbinding/glbinding_api.h>

#include <glbinding/nogl.h>
#include <glbinding/gl/types.h>


namespace gl
{


GLBINDING_API void glConvolutionParametere(GLenum target, GLenum pname, GLenum params);
GLBINDING_API void glConvolutionParametereEXT(GLenum target, GLenum pname, GLenum params);

GLBINDING_API void glFoge(GLenum pname, GLenum param);
GLBINDING_API void glFogev(GLenum pname, const GLenum * params);

GLBINDING_API void glGetBufferParameterbv(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetBufferParameterbvARB(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetBufferParameterev(GLenum target, GLenum pname, GLenum * params);
GLBINDING_API void glGetBufferParameterevARB(GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetConvolutionParameterev(GLenum target, GLenum pname, GLenum * params);
GLBINDING_API void glGetConvolutionParameterevEXT(GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetEnumv(GLenum pname, GLenum * data);
GLBINDING_API void glGetEnumi_v(GLenum target, GLuint index, GLenum * data);

GLBINDING_API void glGetFramebufferAttachmentParameterev(GLenum target, GLenum attachment, GLenum pname, GLenum * params);
GLBINDING_API void glGetFramebufferAttachmentParameterevEXT(GLenum target, GLenum attachment, GLenum pname, GLenum * params);

GLBINDING_API void glGetFramebufferParameterbv(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetFramebufferParameterbvEXT(GLuint framebuffer, GLenum pname, GLboolean * params);

GLBINDING_API void glGetMinmaxParameterev(GLenum target, GLenum pname, GLenum * params);
GLBINDING_API void glGetMinmaxParameterevEXT(GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetNamedBufferParameterbv(GLuint buffer, GLenum pname, GLboolean * params);
GLBINDING_API void glGetNamedBufferParameterbvEXT(GLuint buffer, GLenum pname, GLboolean * params);
GLBINDING_API void glGetNamedBufferParameterev(GLuint buffer, GLenum pname, GLenum * params);
GLBINDING_API void glGetNamedBufferParameterevEXT(GLuint buffer, GLenum pname, GLenum * params);

GLBINDING_API void glGetNamedFramebufferAttachmentParameterev(GLuint framebuffer, GLenum attachment, GLenum pname, GLenum * params);
GLBINDING_API void glGetNamedFramebufferAttachmentParameterevEXT(GLuint framebuffer, GLenum attachment, GLenum pname, GLenum * params);

GLBINDING_API void glGetNamedFramebufferParameterbv(GLuint framebuffer, GLenum pname, GLboolean * param);
GLBINDING_API void glGetNamedFramebufferParameterbvEXT(GLuint framebuffer, GLenum pname, GLboolean * params);

GLBINDING_API void glGetNamedProgrambvEXT(GLuint program, GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetNamedProgramevEXT(GLuint program, GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetNamedRenderbufferParameterev(GLuint renderbuffer, GLenum pname, GLenum * params);
GLBINDING_API void glGetNamedRenderbufferParameterevEXT(GLuint renderbuffer, GLenum pname, GLenum * params);

GLBINDING_API void glGetNamedStringevARB(GLint namelen, const GLchar * name, GLenum pname, GLenum * params);

GLBINDING_API void glGetObjectParameterbvARB(GLhandleARB obj, GLenum pname, GLboolean * params);
GLBINDING_API void glGetObjectParameterevARB(GLhandleARB obj, GLenum pname, GLenum * params);

GLBINDING_API void glGetProgrambv(GLuint program, GLenum pname, GLboolean * params);
GLBINDING_API void glGetProgrambvARB(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetProgramev(GLuint program, GLenum pname, GLenum * params);
GLBINDING_API void glGetProgramevARB(GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetQueryIndexedbv(GLenum target, GLuint index, GLenum pname, GLboolean * params);

GLBINDING_API void glGetQueryObjectbv(GLuint id, GLenum pname, GLboolean * params);
GLBINDING_API void glGetQueryObjectbvARB(GLuint id, GLenum pname, GLboolean * params);

GLBINDING_API void glGetQuerybv(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetQuerybvARB(GLenum target, GLenum pname, GLboolean * params);

GLBINDING_API void glGetRenderbufferParameterev(GLenum target, GLenum pname, GLenum * params);
GLBINDING_API void glGetRenderbufferParameterevEXT(GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetSamplerParameterIev(GLuint sampler, GLenum pname, GLenum * params);
GLBINDING_API void glGetSamplerParameterev(GLuint sampler, GLenum pname, GLenum * params);

GLBINDING_API void glGetShaderbv(GLuint shader, GLenum pname, GLboolean * params);
GLBINDING_API void glGetShaderev(GLuint shader, GLenum pname, GLenum * params);

GLBINDING_API void glGetTexEnvbv(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTexEnvev(GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetTexGenev(GLenum coord, GLenum pname, GLenum * params);

GLBINDING_API void glGetTexLevelParameterbv(GLenum target, GLint level, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTexLevelParameterev(GLenum target, GLint level, GLenum pname, GLenum * params);

GLBINDING_API void glGetTexParameterIbv(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTexParameterIbvEXT(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTexParameterbv(GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTexParameterIev(GLenum target, GLenum pname, GLenum * params);
GLBINDING_API void glGetTexParameterIevEXT(GLenum target, GLenum pname, GLenum * params);
GLBINDING_API void glGetTexParameterev(GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetTextureLevelParameterbv(GLuint texture, GLint level, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTextureLevelParameterbvEXT(GLuint texture, GLenum target, GLint level, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTextureLevelParameterev(GLuint texture, GLint level, GLenum pname, GLenum * params);
GLBINDING_API void glGetTextureLevelParameterevEXT(GLuint texture, GLenum target, GLint level, GLenum pname, GLenum * params);

GLBINDING_API void glGetTextureParameterIbv(GLuint texture, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTextureParameterIbvEXT(GLuint texture, GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTextureParameterbv(GLuint texture, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTextureParameterbvEXT(GLuint texture, GLenum target, GLenum pname, GLboolean * params);
GLBINDING_API void glGetTextureParameterIev(GLuint texture, GLenum pname, GLenum * params);
GLBINDING_API void glGetTextureParameterIevEXT(GLuint texture, GLenum target, GLenum pname, GLenum * params);
GLBINDING_API void glGetTextureParameterev(GLuint texture, GLenum pname, GLenum * params);
GLBINDING_API void glGetTextureParameterevEXT(GLuint texture, GLenum target, GLenum pname, GLenum * params);

GLBINDING_API void glGetTransformFeedbackbv(GLuint xfb, GLenum pname, GLboolean * param);

GLBINDING_API void glGetVertexArrayIndexedbv(GLuint vaobj, GLuint index, GLenum pname, GLboolean * param);
GLBINDING_API void glGetVertexArrayIndexedev(GLuint vaobj, GLuint index, GLenum pname, GLenum * param);

GLBINDING_API void glGetVertexAttribIbv(GLuint index, GLenum pname, GLboolean * params);
GLBINDING_API void glGetVertexAttribIbvEXT(GLuint index, GLenum pname, GLboolean * params);
GLBINDING_API void glGetVertexAttribbv(GLuint index, GLenum pname, GLboolean * params);
GLBINDING_API void glGetVertexAttribbvARB(GLuint index, GLenum pname, GLboolean * params);
GLBINDING_API void glGetVertexAttribIev(GLuint index, GLenum pname, GLenum * params);
GLBINDING_API void glGetVertexAttribIevEXT(GLuint index, GLenum pname, GLenum * params);
GLBINDING_API void glGetVertexAttribev(GLuint index, GLenum pname, GLenum * params);
GLBINDING_API void glGetVertexAttribevARB(GLuint index, GLenum pname, GLenum * params);

GLBINDING_API void glLightModele(GLenum pname, GLenum param);
GLBINDING_API void glLightModelev(GLenum pname, const GLenum * params);

GLBINDING_API void glMultiTexImage1DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void * pixels);
GLBINDING_API void glMultiTexImage2DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels);
GLBINDING_API void glMultiTexImage3DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels);

GLBINDING_API void glNamedFramebufferParameterb(GLuint framebuffer, GLenum pname, GLboolean param);
GLBINDING_API void glNamedFramebufferParameterbEXT(GLuint framebuffer, GLenum pname, GLboolean param);

GLBINDING_API void glPixelStoreb(GLenum pname, GLboolean param);

GLBINDING_API void glPixelTransferb(GLenum pname, GLboolean param);

GLBINDING_API void glPointParametere(GLenum pname, GLenum param);
GLBINDING_API void glPointParameterev(GLenum pname, const GLenum * params);

GLBINDING_API void glProgramParameterb(GLuint program, GLenum pname, GLboolean value);
GLBINDING_API void glProgramParameterbARB(GLuint program, GLenum pname, GLboolean value);
GLBINDING_API void glProgramParameterbEXT(GLuint program, GLenum pname, GLboolean value);

GLBINDING_API void glProgramUniform1b(GLuint program, GLint location, GLboolean v0);
GLBINDING_API void glProgramUniform1bEXT(GLuint program, GLint location, GLboolean v0);
GLBINDING_API void glProgramUniform1bv(GLuint program, GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glProgramUniform1bvEXT(GLuint program, GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glProgramUniform2b(GLuint program, GLint location, GLboolean v0, GLboolean v1);
GLBINDING_API void glProgramUniform2bEXT(GLuint program, GLint location, GLboolean v0, GLboolean v1);
GLBINDING_API void glProgramUniform2bv(GLuint program, GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glProgramUniform2bvEXT(GLuint program, GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glProgramUniform3b(GLuint program, GLint location, GLboolean v0, GLboolean v1, GLboolean v2);
GLBINDING_API void glProgramUniform3bEXT(GLuint program, GLint location, GLboolean v0, GLboolean v1, GLboolean v2);
GLBINDING_API void glProgramUniform3bv(GLuint program, GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glProgramUniform3bvEXT(GLuint program, GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glProgramUniform4b(GLuint program, GLint location, GLboolean v0, GLboolean v1, GLboolean v2, GLboolean v3);
GLBINDING_API void glProgramUniform4bEXT(GLuint program, GLint location, GLboolean v0, GLboolean v1, GLboolean v2, GLboolean v3);
GLBINDING_API void glProgramUniform4bv(GLuint program, GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glProgramUniform4bvEXT(GLuint program, GLint location, GLsizei count, const GLboolean * value);

GLBINDING_API void glProgramUniform1e(GLuint program, GLint location, GLenum v0);
GLBINDING_API void glProgramUniform1eEXT(GLuint program, GLint location, GLenum v0);
GLBINDING_API void glProgramUniform1ev(GLuint program, GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glProgramUniform1evEXT(GLuint program, GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glProgramUniform2e(GLuint program, GLint location, GLenum v0, GLenum v1);
GLBINDING_API void glProgramUniform2eEXT(GLuint program, GLint location, GLenum v0, GLenum v1);
GLBINDING_API void glProgramUniform2ev(GLuint program, GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glProgramUniform2evEXT(GLuint program, GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glProgramUniform3e(GLuint program, GLint location, GLenum v0, GLenum v1, GLenum v2);
GLBINDING_API void glProgramUniform3eEXT(GLuint program, GLint location, GLenum v0, GLenum v1, GLenum v2);
GLBINDING_API void glProgramUniform3ev(GLuint program, GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glProgramUniform3evEXT(GLuint program, GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glProgramUniform4e(GLuint program, GLint location, GLenum v0, GLenum v1, GLenum v2, GLenum v3);
GLBINDING_API void glProgramUniform4eEXT(GLuint program, GLint location, GLenum v0, GLenum v1, GLenum v2, GLenum v3);
GLBINDING_API void glProgramUniform4ev(GLuint program, GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glProgramUniform4evEXT(GLuint program, GLint location, GLsizei count, const GLenum * value);

GLBINDING_API void glSamplerParameterIev(GLuint sampler, GLenum pname, const GLenum * param);
GLBINDING_API void glSamplerParametere(GLuint sampler, GLenum pname, GLenum param);
GLBINDING_API void glSamplerParameterev(GLuint sampler, GLenum pname, const GLenum * param);

GLBINDING_API void glTexEnvb(GLenum target, GLenum pname, GLboolean param);
GLBINDING_API void glTexEnvbv(GLenum target, GLenum pname, const GLboolean * params);
GLBINDING_API void glTexEnve(GLenum target, GLenum pname, GLenum param);
GLBINDING_API void glTexEnvev(GLenum target, GLenum pname, const GLenum * params);

GLBINDING_API void glTexGene(GLenum coord, GLenum pname, GLenum param);
GLBINDING_API void glTexGenev(GLenum coord, GLenum pname, const GLenum * params);

GLBINDING_API void glTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void * pixels);
GLBINDING_API void glTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels);
GLBINDING_API void glTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels);

GLBINDING_API void glTextureImage1DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void * pixels);
GLBINDING_API void glTextureImage2DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels);
GLBINDING_API void glTextureImage3DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels);

GLBINDING_API void glTexParameterIbv(GLenum target, GLenum pname, const GLboolean * params);
GLBINDING_API void glTexParameterIbvEXT(GLenum target, GLenum pname, const GLboolean * params);
GLBINDING_API void glTexParameterb(GLenum target, GLenum pname, GLboolean param);
GLBINDING_API void glTexParameterbv(GLenum target, GLenum pname, const GLboolean * params);
GLBINDING_API void glTexParameterIev(GLenum target, GLenum pname, const GLenum * params);
GLBINDING_API void glTexParameterIevEXT(GLenum target, GLenum pname, const GLenum * params);
GLBINDING_API void glTexParametere(GLenum target, GLenum pname, GLenum param);
GLBINDING_API void glTexParameterev(GLenum target, GLenum pname, const GLenum * params);

GLBINDING_API void glTextureParameterIbv(GLuint texture, GLenum pname, const GLboolean * params);
GLBINDING_API void glTextureParameterIbvEXT(GLuint texture, GLenum target, GLenum pname, const GLboolean * params);
GLBINDING_API void glTextureParameterb(GLuint texture, GLenum pname, GLboolean param);
GLBINDING_API void glTextureParameterbEXT(GLuint texture, GLenum target, GLenum pname, GLboolean param);
GLBINDING_API void glTextureParameterbv(GLuint texture, GLenum pname, const GLboolean * param);
GLBINDING_API void glTextureParameterbvEXT(GLuint texture, GLenum target, GLenum pname, const GLboolean * params);
GLBINDING_API void glTextureParameterIev(GLuint texture, GLenum pname, const GLenum * params);
GLBINDING_API void glTextureParameterIevEXT(GLuint texture, GLenum target, GLenum pname, const GLenum * params);
GLBINDING_API void glTextureParametere(GLuint texture, GLenum pname, GLenum param);
GLBINDING_API void glTextureParametereEXT(GLuint texture, GLenum target, GLenum pname, GLenum param);
GLBINDING_API void glTextureParameterev(GLuint texture, GLenum pname, const GLenum * param);
GLBINDING_API void glTextureParameterevEXT(GLuint texture, GLenum target, GLenum pname, const GLenum * params);

GLBINDING_API void glUniform1b(GLint location, GLboolean v0);
GLBINDING_API void glUniform1bARB(GLint location, GLboolean v0);
GLBINDING_API void glUniform1bv(GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glUniform1bvARB(GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glUniform2b(GLint location, GLboolean v0, GLboolean v1);
GLBINDING_API void glUniform2bARB(GLint location, GLboolean v0, GLboolean v1);
GLBINDING_API void glUniform2bv(GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glUniform2bvARB(GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glUniform3b(GLint location, GLboolean v0, GLboolean v1, GLboolean v2);
GLBINDING_API void glUniform3bARB(GLint location, GLboolean v0, GLboolean v1, GLboolean v2);
GLBINDING_API void glUniform3bv(GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glUniform3bvARB(GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glUniform4b(GLint location, GLboolean v0, GLboolean v1, GLboolean v2, GLboolean v3);
GLBINDING_API void glUniform4bARB(GLint location, GLboolean v0, GLboolean v1, GLboolean v2, GLboolean v3);
GLBINDING_API void glUniform4bv(GLint location, GLsizei count, const GLboolean * value);
GLBINDING_API void glUniform4bvARB(GLint location, GLsizei count, const GLboolean * value);

GLBINDING_API void glUniform1e(GLint location, GLenum v0);
GLBINDING_API void glUniform1eARB(GLint location, GLenum v0);
GLBINDING_API void glUniform1ev(GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glUniform1evARB(GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glUniform2e(GLint location, GLenum v0, GLenum v1);
GLBINDING_API void glUniform2eARB(GLint location, GLenum v0, GLenum v1);
GLBINDING_API void glUniform2ev(GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glUniform2evARB(GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glUniform3e(GLint location, GLenum v0, GLenum v1, GLenum v2);
GLBINDING_API void glUniform3eARB(GLint location, GLenum v0, GLenum v1, GLenum v2);
GLBINDING_API void glUniform3ev(GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glUniform3evARB(GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glUniform4e(GLint location, GLenum v0, GLenum v1, GLenum v2, GLenum v3);
GLBINDING_API void glUniform4eARB(GLint location, GLenum v0, GLenum v1, GLenum v2, GLenum v3);
GLBINDING_API void glUniform4ev(GLint location, GLsizei count, const GLenum * value);
GLBINDING_API void glUniform4evARB(GLint location, GLsizei count, const GLenum * value);


} // namespace gl
