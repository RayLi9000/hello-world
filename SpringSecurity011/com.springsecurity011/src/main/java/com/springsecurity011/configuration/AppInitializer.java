package com.springsecurity011.configuration;

import org.springframework.web.servlet.support.AbstractAnnotationConfigDispatcherServletInitializer;

public class AppInitializer extends AbstractAnnotationConfigDispatcherServletInitializer{

	@Override
	protected Class<?>[] getRootConfigClasses() 
	{
		return new Class[] {SpringMvcConfig.class};
	}

	@Override
	protected Class<?>[] getServletConfigClasses()
	{
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	protected String[] getServletMappings()
	{
		return new String[] {"/"};
	}

}
