#include "NetKinectArray.h"
#include "squish/squish.h"

#include <FileBuffer.h>
#include <Timer.h>
#include <TextureArray.h>

#include <FrameTimeAdjuster.h>

#include <KinectCalibrationFile.h>
#include <gl_util.h>
#include <Shader.h>
#include <UniformSet.h>
#include <TextureManager.h>
#include <Viewport.h>
#include <gloostHelper.h>

#include <timevalue.h>
#include <clock.h>

#include <DXTCompressor.h>


#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <iostream>
#include <vector>
#include <string>

#include <zmq.hpp>

#include <fstream>
#include <sstream>




namespace kinect{

  /*static*/ bool NetKinectArray::s_glewInit = false;

  NetKinectArray::NetKinectArray(const char* config, bool readfromfile)
    : m_width(0),
      m_widthc(0),
      m_height(0),
      m_heightc(0),
      m_numLayers(0),
      m_kinectcs(),
      m_colorArray(0),
      m_depthArray(0),
      m_colorArray_back(0),
      m_depthArray_back(0),
      m_shader_bf(0),
      m_uniforms_bf(0),
      m_fboID(0),
      m_gaussID(0),
      m_colorsize(0),
      m_depthsize(0),
      m_colorsCPU3(),
      m_depthsCPU3(),
      m_mutex(new boost::mutex),
      m_readThread(0),
      m_running(true),
      m_serverport(""),
      m_trigger(10), // means no update at the beginning
      m_artl(new ARTListener(config)),
      m_isrecording(false),
      m_readfromfile(readfromfile),
      m_isphoto(false),
      m_config(config),
      depth_compression_lex(false),
      depth_compression_ratio(100.0f)
  {

    if(!s_glewInit){
      // initialize GLEW
      if (GLEW_OK != glewInit()){
	/// ... or die trying
	std::cout << "'glewInit()' failed." << std::endl;
	exit(0);
      }
      else{
	s_glewInit = true;
      }
    }

    std::string tmp_config(config);
    if(std::string::npos == tmp_config.find("kinect_surface")){
      m_isrecording = true;
    }


    std::ifstream in(config);
    std::string token;
    while(in >> token){
      if(token == "serverport"){
	in >> m_serverport;
      }
      if(token == "kinect"){
	in >> token;
	KinectCalibrationFile* calib = new KinectCalibrationFile(token);
	calib->parse();
	m_kinectcs.push_back(calib);
      }
    }
    in.close();

    init();

    if(m_readfromfile){
      readFromFiles();
    }
    else{
      m_readThread = new boost::thread(boost::bind(&NetKinectArray::readLoop, this));
    }

  }


  

  NetKinectArray::NetKinectArray(std::vector<KinectCalibrationFile*>& calibs, bool readfromfile)
    : m_width(0),
      m_widthc(0),
      m_height(0),
      m_heightc(0),
      m_numLayers(0),
      m_kinectcs(calibs),
      m_colorArray(0),
      m_depthArray(0),
      m_colorArray_back(0),
      m_depthArray_back(0),
      m_shader_bf(0),
      m_uniforms_bf(0),
      m_fboID(0),
      m_gaussID(0),
      m_colorsize(0),
      m_depthsize(0),
      m_colorsCPU3(),
      m_depthsCPU3(),
      m_mutex(new boost::mutex),
      m_readThread(0),
      m_running(true),
      m_serverport(""),
      m_trigger(10), // means no update at the beginning
      m_artl(0),
      m_isrecording(false),
      m_readfromfile(readfromfile),
      m_isphoto(false),
      m_config(""),
      depth_compression_lex(false),
      depth_compression_ratio(100.0f)
  {

    if(!s_glewInit){
      // initialize GLEW
      if (GLEW_OK != glewInit()){
	/// ... or die trying
	std::cout << "'glewInit()' failed." << std::endl;
	exit(0);
      }
      else{
	s_glewInit = true;
      }
    }
    
    init();

  }


  bool
  NetKinectArray::init(){
    m_numLayers = m_kinectcs.size();
    m_width   = m_kinectcs[0]->getWidth();
    m_widthc  = m_kinectcs[0]->getWidthC();
    m_height  = m_kinectcs[0]->getHeight();
    m_heightc = m_kinectcs[0]->getHeightC();

    
    if(m_kinectcs[0]->isCompressedRGB() == 1){
      mvt::DXTCompressor dxt;
      dxt.init(m_kinectcs[0]->getWidthC(), m_kinectcs[0]->getHeightC(), FORMAT_DXT1);
      m_colorsize = dxt.getStorageSize();
    }
    else if(m_kinectcs[0]->isCompressedRGB() == 5){
      std::cerr << "NetKinectArray: using DXT5" << std::endl;
      m_colorsize = 307200;
    }
    else{
      m_colorsize = m_widthc * m_heightc * 3 * sizeof(unsigned char);
    }

    m_colorsCPU3.matrixdata_back = new float [ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix)];
    m_colorsCPU3.matrixdata_front = new float [ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix)];

    m_colorsCPU3.size = m_colorsize * m_numLayers;
    m_colorsCPU3.needSwap = false;

    glGenBuffers(1,&m_colorsCPU3.frontID);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,m_colorsCPU3.frontID);
    glBufferData(GL_PIXEL_PACK_BUFFER, m_colorsCPU3.size, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

    glGenBuffers(1,&m_colorsCPU3.backID);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,m_colorsCPU3.backID);
    glBufferData(GL_PIXEL_PACK_BUFFER, m_colorsCPU3.size, 0, GL_DYNAMIC_DRAW);
    m_colorsCPU3.back = (unsigned char *) glMapBufferRange(GL_PIXEL_PACK_BUFFER,0 /*offset*/, m_colorsCPU3.size /*length*/, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

    if(m_kinectcs[0]->isCompressedDepth()){
      m_depthsCPU3.size = m_width * m_height * m_numLayers * sizeof(unsigned char);
      m_depthsize =  m_width * m_height * sizeof(unsigned char);
    }
    else{
      m_depthsCPU3.size = m_width * m_height * m_numLayers * sizeof(float);
      m_depthsize =  m_width * m_height * sizeof(float);
    }
    
    m_depthsCPU3.needSwap = false;

    for(unsigned i = 0; i < m_numLayers; ++i){
      m_depthsCPU3.current_poses.push_back(gloost::Matrix());
    }
    current_poses = m_depthsCPU3.current_poses;

    glGenBuffers(1,&m_depthsCPU3.frontID);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,m_depthsCPU3.frontID);
    glBufferData(GL_PIXEL_PACK_BUFFER, m_depthsCPU3.size, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

    glGenBuffers(1,&m_depthsCPU3.backID);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,m_depthsCPU3.backID);
    glBufferData(GL_PIXEL_PACK_BUFFER, m_depthsCPU3.size, 0, GL_DYNAMIC_DRAW);
    m_depthsCPU3.back = (unsigned char *) glMapBufferRange(GL_PIXEL_PACK_BUFFER,0 /*offset*/, m_depthsCPU3.size /*length*/, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);








    /* kinect color: GL_RGB32F, GL_RGB, GL_FLOAT*/
    /* kinect depth: GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT*/
    //m_colorArray = new mvt::TextureArray(m_width, m_height, m_numLayers, GL_RGB32F, GL_RGB, GL_FLOAT);
    if(m_kinectcs[0]->isCompressedRGB() == 1){
      m_colorArray = new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, m_colorsize);
    }
    else if(m_kinectcs[0]->isCompressedRGB() == 5){
      m_colorArray = new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, m_colorsize);
    }
    else{
      m_colorArray = new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
    }

    m_depthArray = new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT);
    
    m_colorArray->getGLHandle();
    check_gl_errors("after m_colorArray->getGLHandle()", false);
    m_depthArray->getGLHandle();
    check_gl_errors("after m_depthArray->getGLHandle()", false);


    m_colorArray_back = new mvt::TextureArray(m_widthc, m_heightc, m_numLayers, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);

    if(m_kinectcs[0]->isCompressedDepth()){
      m_depthArray_back = new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE, GL_RED, GL_UNSIGNED_BYTE);
    }
    else{
      m_depthArray_back = new mvt::TextureArray(m_width, m_height, m_numLayers, GL_LUMINANCE32F_ARB, GL_RED, GL_FLOAT);
    }
    

    m_colorArray_back->getGLHandle();
    m_depthArray_back->getGLHandle();
    glGenFramebuffersEXT(1, &m_fboID);
    m_uniforms_bf = new gloost::UniformSet;


    m_uniforms_bf->set_int("kinect_depths",1);
    //m_uniforms_bf->set_int("kinect_colors",2);
    m_uniforms_bf->set_vec2("texSizeInv", gloost::vec2(1.0f/m_width, 1.0f/m_height));

    reloadShader();


    gloost::TextureManager* texManager = gloost::TextureManager::getInstance();
    if(m_gaussID){
      texManager->dropReference(m_gaussID);
      texManager->cleanUp();
    }

    
    m_gaussID = texManager->createTexture("glsl/gauss.png");
    texManager->getTextureWithoutRefcount(m_gaussID)->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texManager->getTextureWithoutRefcount(m_gaussID)->setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texManager->getTextureWithoutRefcount(m_gaussID)->setTexParameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
    texManager->getTextureWithoutRefcount(m_gaussID)->setTexParameter(GL_TEXTURE_WRAP_T, GL_CLAMP);

    m_uniforms_bf->set_sampler2D("gauss",m_gaussID);




    std::cerr << "NetKinectArray::NetKinectArray: " << this << std::endl;


    return true;
  }

  NetKinectArray::~NetKinectArray(){
    delete m_colorArray;
    delete m_depthArray;
    delete m_colorArray_back;
    delete m_depthArray_back;
    delete m_shader_bf;
    delete m_uniforms_bf;
    glDeleteFramebuffersEXT(1, &m_fboID);


    for(unsigned i = 0; i < m_kinectcs.size(); ++i){
      delete m_kinectcs[i];
    }

    m_colorsCPU3.needSwap = false;
    m_depthsCPU3.needSwap = false;
    m_running = false;
    m_readThread->join();
    delete m_readThread;
    delete m_mutex;


  }


  void
  NetKinectArray::update(bool filter){
    
    

    {
      boost::mutex::scoped_lock lock(*m_mutex);
      //static sensor::Timer t;
      //t.start();

      //t.stop();
      //std::cerr << "upload usec " << t.get().msec() << std::endl;
      if(m_colorsCPU3.needSwap){

	//std::cerr << "downloading" << std::endl;

	glBindBuffer(GL_PIXEL_PACK_BUFFER,m_colorsCPU3.backID);
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER,m_depthsCPU3.backID);
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

	m_colorsCPU3.swap();
	m_depthsCPU3.swap();

	glBindBuffer(GL_PIXEL_PACK_BUFFER,m_colorsCPU3.backID);
	m_colorsCPU3.back = (unsigned char *) glMapBufferRange(GL_PIXEL_PACK_BUFFER,0 /*offset*/, m_colorsCPU3.size /*length*/, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

	glBindBuffer(GL_PIXEL_PACK_BUFFER,m_depthsCPU3.backID);
	m_depthsCPU3.back = (unsigned char *) glMapBufferRange(GL_PIXEL_PACK_BUFFER,0 /*offset*/, m_depthsCPU3.size /*length*/, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,0);

	
	current_poses = m_depthsCPU3.current_poses;
	
	m_artl->get(m_colorsCPU3.matrixdata_front);

	m_colorsCPU3.needSwap = false;
	m_trigger = 0;
      }
      
    }

    if(!filter)
      return;

    ++m_trigger;
    if(1 == m_trigger){
      //std::cerr << "fill" << std::endl;
      m_colorArray->fillLayersFromPBO(m_colorsCPU3.frontID);

      if(0/*m_kinectcs[0]->use_bf*/){ // we do not want to filter here, we filter in KinectSurface!
	m_depthArray_back->fillLayersFromPBO(m_depthsCPU3.frontID);
      }
      else{
	m_depthArray->fillLayersFromPBO(m_depthsCPU3.frontID);
	return;
      }
    }


    if(1 == m_trigger){
      //std::cerr << "bilateral filtering" << std::endl;
      bilateralFilter();

    }



  }


  void
  NetKinectArray::bilateralFilter(){

      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glEnable(GL_DEPTH_TEST);

      gloost::Viewport vp(0,0,m_width,m_height);
      vp.enter();

      for(unsigned i = 0; i < m_kinectcs.size(); ++i){

	//
	GLint current_fbo;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &current_fbo);
	//std::cerr << current_fbo << std::endl;
	// render to depth
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fboID);
	GLenum buffers[] = {GL_COLOR_ATTACHMENT0_EXT/*, GL_COLOR_ATTACHMENT1_EXT*/ };
	glDrawBuffers(1, buffers);
	glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_depthArray->getGLHandle(), 0, i);
	//glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, m_colorArray->getGLHandle(), 0, i);
	//glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  m_ogldepthArray->getGLHandle(), 0, i);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status){
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	  printf("Error:Frame buffer not supported in NetKinectArray::update().\n");
	  break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
	  printf("Error:Frame buffer not supported in NetKinectArray::update().\n");
	  break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Error:Missing color attachment in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Error:All textures attached to frame buffer must have same dimension in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Error:All textures attached to frame buffer must have same format in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Error:Missing draw buffer in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Error:Missing read buffer in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
            printf("Error:Missing layer targets in NetKinectArray::update().\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT:
            printf("Error:Incomplete layer count in NetKinectArray::update().\n");
            break;
	case GL_FRAMEBUFFER_COMPLETE_EXT:
	default:
	  break;
	}

#if 1
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);
#endif

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0,1.0,0.0,1.0,1.0,-1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
    


	m_uniforms_bf->set_int("layer",i);
	m_uniforms_bf->set_bool("compress",m_kinectcs[i]->isCompressedDepth());
	const float near = m_kinectcs[i]->getNear();
	const float far  = m_kinectcs[i]->getFar();
	const float scale = (far - near);
	
	// float d = d_c * scale + near;
	m_uniforms_bf->set_float("scale",scale);
	m_uniforms_bf->set_float("near",near);
	m_uniforms_bf->set_float("scaled_near",scale/255.0);
	m_shader_bf->set();
	m_uniforms_bf->applyToShader(m_shader_bf);

	glActiveTexture(GL_TEXTURE0 + 1);
	m_depthArray_back->bind();
	//glActiveTexture(GL_TEXTURE0 + 2);
	//m_colorArray_back->bind();

	glBegin(GL_QUADS);
	{
	  glTexCoord2f(0.0f, 0.0f);
	  glVertex3f  (0.0f, 0.0f, 0.0f);
	  
	  glTexCoord2f(1.0f, 0.0f);
	  glVertex3f  (1.0f, 0.0f, 0.0f);
	  
	  glTexCoord2f(1.0f, 1.0f);
	  glVertex3f  (1.0f, 1.0f, 0.0f);
	  
	  glTexCoord2f(0.0f, 1.0f);
	  glVertex3f  (0.0f, 1.0f, 0.0f);
	}
	glEnd();

	glActiveTexture(GL_TEXTURE0);
	m_shader_bf->disable();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, current_fbo);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

      }
      vp.leave();
      glPopAttrib();
  }


  void
  NetKinectArray::bindToTextureUnits(GLenum start_texture_unit){
    
    glActiveTexture(start_texture_unit/*GL_TEXTURE0*/);
    m_colorArray->bind();
    glActiveTexture(start_texture_unit + 1);
    m_depthArray->bind();
    
  }

  unsigned
  NetKinectArray::getWidth(){
    return m_width;
  }
  unsigned
  NetKinectArray::getWidthC(){
    return m_widthc;
  }
  unsigned
  NetKinectArray::getHeight(){
    return m_height;
  }
  unsigned
  NetKinectArray::getHeightC(){
    return m_heightc;
  }

  unsigned
  NetKinectArray::getNumLayers(){
    return m_numLayers;
  }


  std::vector<KinectCalibrationFile*>&
  NetKinectArray::getCalibs(){
    return m_kinectcs;
  }


  mvt::TextureArray*
  NetKinectArray::getDepthArrayBack(){
    return m_depthArray_back;
  }

  mvt::TextureArray*
  NetKinectArray::getDepthArray(){
    return m_depthArray;
  }


  void
  NetKinectArray::readLoop(){
    // open multicast listening connection to server and port

    zmq::context_t ctx(1); // means single threaded
    zmq::socket_t  socket(ctx, ZMQ_SUB); // means a subscriber

    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    uint64_t hwm = 1;
    socket.setsockopt(ZMQ_HWM,&hwm, sizeof(hwm));

    std::string endpoint("tcp://" + m_serverport);
    socket.connect(endpoint.c_str());

 
    //const unsigned pixelcountc = m_kinectcs[0]->getWidthC() * m_kinectcs[0]->getHeightC();
    const unsigned pixelcount = m_kinectcs[0]->getWidth() * m_kinectcs[0]->getHeight();
    
    const unsigned colorsize = m_colorsize;
    const unsigned depthsize = m_depthsize;//pixelcount * sizeof(float);

    kinect::FrameTimeAdjuster fta;
    bool drop = false;
    sensor::timevalue ts(sensor::clock::time());
    unsigned framenr = 0;
    unsigned lastframenr = 99;

    while(m_running){



      zmq::message_t zmqm((colorsize + depthsize) * m_kinectcs.size());
      
      socket.recv(&zmqm); // blocking
      

      if(!drop){


	while(m_colorsCPU3.needSwap){
	  ;
	}

	unsigned offset = 0;
	// receive data
        const unsigned number_of_kinects = m_kinectcs.size(); // is 5 in the current example
        // this loop goes over each kinect like K1_frame_1 K2_frame_1 K3_frame_1 
	for(unsigned i = 0; i < number_of_kinects; ++i){
	  memcpy((unsigned char*) m_colorsCPU3.back + i*colorsize , (unsigned char*) zmqm.data() + offset, colorsize);
	  offset += colorsize;
	  memcpy((unsigned char*) m_depthsCPU3.back + i*depthsize , (unsigned char*) zmqm.data() + offset, depthsize);
	  
	  if(m_kinectcs[i]->isSensored()){
	    // This is new and works only for one Kinect
	    memcpy(m_depthsCPU3.current_poses[i].data(), (unsigned char*) zmqm.data() + offset, 16 * sizeof(float));
	  }
	  
	  offset += depthsize;
	}


      }



      
      const unsigned ts_address = ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix);
      if(!drop){
	memcpy((void *) m_colorsCPU3.matrixdata_back, zmqm.data(), ts_address /*ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix)*/);
      }
      memcpy(&ts,           zmqm.data() + ts_address, sizeof(sensor::timevalue));
      const unsigned framenr_address = ts_address + sizeof(sensor::timevalue);
      unsigned curr_framenr;
      memcpy(&curr_framenr, zmqm.data() + framenr_address, sizeof(curr_framenr));
      //std::cerr << "received frame " << curr_framenr << std::endl;
      if((curr_framenr < framenr) || (lastframenr == framenr)){
	bool tmp_isphoto = (((framenr - curr_framenr) < 29) && m_isrecording) ? true : false;
	
	if((tmp_isphoto) || (lastframenr == framenr)){

 	  // check if ".rec" file is avaible
	  size_t pos = m_config.rfind(".");
	  std::string photoname = m_config.substr(0,pos);
	  photoname = std::string("./recordings/") + photoname + ".photo";
	  struct stat attrib;
	  int ret = stat(photoname.c_str(), &attrib);
	  if(0 == ret){
	    m_isphoto = true; // maybe sync system?//true;
	  }


	}
      }
      lastframenr = framenr;
      framenr = curr_framenr;
      
      //std::cerr << "is photo " << (int) m_isphoto << std::endl;





      if(!drop){ // swap
	boost::mutex::scoped_lock lock(*m_mutex);
	m_colorsCPU3.needSwap = true;
	
      }

      //drop = fta.putTime(ts);
#if 0
      if(m_isrecording){
	bool tmp_drop = fta.putTime(ts);
      }
#endif
    }
  }



  void
  NetKinectArray::reloadShader(){
    if(m_shader_bf){
      delete m_shader_bf;
    }
    m_shader_bf = new gloost::Shader("glsl/bf.vs","glsl/bf.fs");

    for(unsigned i = 0; i < m_kinectcs.size(); ++i){
      m_kinectcs[i]->parse();
    }

  }


  void
  NetKinectArray::drawGeometry(){
    //m_artl->draw();
  }

  void
  NetKinectArray::writeCurrentTexture(std::string prefix){
    //depths
    if (m_kinectcs[0]->isCompressedDepth())
    {
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_depthArray->getGLHandle());
      int width, height, depth;
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &depth);
      
      std::vector<unsigned char> depths;
      depths.resize(width*height*depth);
      
      glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RED, GL_UNSIGNED_BYTE, (void*)&depths[0]); 
      
      int offset = 0;
      
      for (unsigned int k = 0; k < depth; ++k)
      {
        std::stringstream sstr;
        sstr << "output/" << prefix << "_d_" << k << ".bmp";
        std::string filename (sstr.str());
        std::cout << "writing depth texture for kinect " << k << " to file " << filename << std::endl;
 
        offset += width*height;
        
        writeBMP(filename, depths, offset, 1);
        offset += width*height;
      }
    }
    else
    {
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_depthArray->getGLHandle());
      int width, height, depth;
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &depth);
      
      std::vector<float> depthsTmp;
      depthsTmp.resize(width*height*depth);
      
      glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RED, GL_FLOAT, (void*)&depthsTmp[0]); 
      
      std::vector<unsigned char> depths;
      depths.resize(depthsTmp.size());
      
      for (unsigned int i = 0; i < width*height*depth; ++i)
      {
        depths[i] = (unsigned char)depthsTmp[i] * 255.0f;
      }
      
      int offset = 0;
      
      for (unsigned int k = 0; k < depth; ++k)
      {
        std::stringstream sstr;
        sstr << "output/" << prefix << "_d_" << k << ".bmp";
        std::string filename (sstr.str());
        std::cout << "writing depth texture for kinect " << k << " to file " << filename << " (values are compressed to 8bit)" << std::endl;

        writeBMP(filename, depths, offset, 1);
        offset += width*height;
      }
    }
    
    //color
    if (m_kinectcs[0]->isCompressedRGB() == 1)
    {
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_colorArray->getGLHandle());
      int size;
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size);
      
      std::vector<unsigned char> data;
      data.resize(size);
      
      glGetCompressedTexImage(GL_TEXTURE_2D_ARRAY, 0, (void*)&data[0]);
      
      std::vector<unsigned char> depths;
      depths.resize(4*m_widthc*m_heightc);
    
      int offset = 0;
      
      for (unsigned int k = 0; k < getNumLayers(); ++k)
      {
        squish::DecompressImage (&depths[0], m_widthc, m_heightc, &data[k*m_colorsize], squish::kDxt1);
        
        std::stringstream sstr;
        sstr << "output/" << prefix << "_col_" << k << ".bmp";
        std::string filename (sstr.str());
        std::cout << "writing color texture for kinect " << k << " to file " << filename << std::endl;

        writeBMP(filename, depths, 0, 4);
      }
    }
    else
    {
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      
      glBindTexture(GL_TEXTURE_2D_ARRAY,m_colorArray->getGLHandle());
      int width, height, depth;
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
      glGetTexLevelParameteriv (GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &depth);
      
      std::vector<unsigned char> depths;
      depths.resize(3*width*height*depth);
      
      glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)&depths[0]); 
      
      int offset = 0;
      
      for (unsigned int k = 0; k < depth; ++k)
      {
        std::stringstream sstr;
        sstr << "output/" << prefix << "_col_" << k << ".bmp";
        std::string filename (sstr.str());
        std::cout << "writing color texture for kinect " << k << " to file " << filename << std::endl;

        writeBMP(filename, depths, offset, 3);
        offset += 3 * width*height;
      }
    }

  }
  
  // no universal use! very unflexible, resolution depth = resolution color, no row padding
  void NetKinectArray::writeBMP(std::string filename, std::vector<unsigned char> const& data, unsigned int offset, unsigned int bytesPerPixel)
  {
    std::ofstream file (filename, std::ofstream::binary);
    char c;
    short s;
    int i;
    
    c = 'B'; file.write(&c, 1);
    c = 'M'; file.write(&c, 1);
    i = m_widthc * m_heightc * 3 + 54; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i,4);
    i = 54; file.write((char const*) &i, 4);
    i = 40; file.write((char const*) &i, 4);
    i = m_widthc; file.write((char const*) &i, 4);
    i = m_heightc; file.write((char const*) &i, 4);
    s = 1; file.write((char const*) &s, 2);
    s = 24; file.write((char const*) &s, 2);
    i = 0; file.write((char const*) &i, 4);
    i = m_widthc * m_heightc * 3; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i, 4);
    i = 0; file.write((char const*) &i, 4);
    
    
    for (unsigned int h = m_heightc; h > 0; --h)
    {
    	for (unsigned int w = 0; w < m_widthc * bytesPerPixel; w += bytesPerPixel)
    	{
    	  if (bytesPerPixel == 1)
          {
            file.write((char const*) &data[offset + w + (h-1) * m_widthc * bytesPerPixel], 1);
            file.write((char const*) &data[offset + w + (h-1) * m_widthc * bytesPerPixel], 1);
            file.write((char const*) &data[offset + w + (h-1) * m_widthc * bytesPerPixel], 1);
          }
          else if (bytesPerPixel == 3 || bytesPerPixel == 4)
          {
            file.write((char const*) &data[offset + w+2 + (h-1) * m_widthc * bytesPerPixel], 1);
            file.write((char const*) &data[offset + w+1 + (h-1) * m_widthc * bytesPerPixel], 1);
            file.write((char const*) &data[offset + w+0 + (h-1) * m_widthc * bytesPerPixel], 1);  
          }
    	}
    }

    file.close();
  }
  
  gloost::Matrix NetKinectArray::getArtlsensorMatrix(unsigned int sensorNumber) const
  {
//  	gloost::Matrix mat[ARTLISTENERNUMSENSORS] {};
//	m_artl->fill(&mat[0]);
//	
//	return mat[sensorNumber];
	
	
//  	gloost::Matrix* mat2 = new gloost::Matrix[ARTLISTENERNUMSENSORS];
//	m_artl->fill(&(mat2[0]));
//	gloost::Matrix mat3 (mat2[sensorNumber]);
////	gloost::Matrix mat3;
//	delete[] mat2;
//	return mat3;
	
	// 18 statt 16 da vptr in matrix class
	return gloost::Matrix(&m_colorsCPU3.matrixdata_front[sensorNumber*18 + 2]);
  }

  ARTListener*
  NetKinectArray::getARTL(){
    return m_artl;
  }


  void
  NetKinectArray::readFromFiles(){
    std::vector<sys::FileBuffer*> fbs;


    for(unsigned i = 0 ; i < m_kinectcs.size(); ++i){
      std::string yml(m_kinectcs[i]->_filePath);
      std::string base((const char*) basename((char *) yml.c_str()));
      base.replace( base.end() - 4, base.end(), "");
      std::string filename = std::string("recordings/" + base + ".stream");

      fbs.push_back(new sys::FileBuffer(filename.c_str()));
      if(!fbs.back()->open("r")){
	std::cerr << "error opening " << filename << " exiting..." << std::endl;
	exit(1);
      }
      fbs.back()->setLooping(/*true*/false);
    }

    
    const unsigned colorsize = m_colorsize;
    const unsigned depthsize = m_depthsize;






    while(m_colorsCPU3.needSwap && m_depthsCPU3.needSwap){
      ;
    }

    unsigned offset = 0;
    // receive data
    for(unsigned i = 0; i < m_kinectcs.size(); ++i){
      //memcpy((unsigned char*) m_colorsCPU3.back + i*colorsize , (unsigned char*) zmqm.data() + offset, colorsize);
      fbs[i]->read((unsigned char*) m_colorsCPU3.back + i*colorsize, colorsize);
      offset += colorsize;

      //memcpy((unsigned char*) m_depthsCPU3.back + i*depthsize , (unsigned char*) zmqm.data() + offset, depthsize);
      fbs[i]->read((unsigned char*) m_depthsCPU3.back + i*depthsize, depthsize);


      
      if(m_kinectcs[i]->isSensored()){
	// This is new and works only for one Kinect
	std::cerr << "NetKinectArray:: ERROR: this is not supported right now"<< std::endl;
      }
      
      offset += depthsize;
    }
      
    const unsigned ts_address = ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix);
    
    memcpy((void *) m_colorsCPU3.matrixdata_back, m_colorsCPU3.back, ts_address /*ARTLISTENERNUMSENSORS * sizeof(gloost::Matrix)*/);
    
      

    { // swap
      boost::mutex::scoped_lock lock(*m_mutex);
      m_colorsCPU3.needSwap = true;
      m_depthsCPU3.needSwap = true;
    }
 
  }



  bool
  NetKinectArray::isRecording(){
    return m_isrecording;
  }

  bool
  NetKinectArray::isPhoto(){
    return m_isphoto;
  }


}