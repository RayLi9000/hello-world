package com.springsecurity011.configuration;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.MessageSource;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.support.ResourceBundleMessageSource;
import org.springframework.format.FormatterRegistry;
import org.springframework.web.servlet.config.annotation.EnableWebMvc;
import org.springframework.web.servlet.config.annotation.PathMatchConfigurer;
import org.springframework.web.servlet.config.annotation.ResourceHandlerRegistry;
import org.springframework.web.servlet.config.annotation.ViewResolverRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurerAdapter;
import org.springframework.web.servlet.view.InternalResourceViewResolver;
import org.springframework.web.servlet.view.JstlView;

import com.springsecurity011.converter.RoleToUserProfileConverter;

@Configuration
@EnableWebMvc
@ComponentScan(basePackages = "com.springsecurity011")
public class SpringMvcConfig extends WebMvcConfigurerAdapter
{
	@Autowired
	RoleToUserProfileConverter roleToUserProfileConverter;
	
	//配置视图解析器
	@Override
	public void configureViewResolvers(ViewResolverRegistry registry)
	{
		InternalResourceViewResolver viewResolver = new InternalResourceViewResolver();
		viewResolver.setViewClass(JstlView.class);
		viewResolver.setPrefix("/WEB-INF/views/");
		viewResolver.setSuffix(".jsp");
		registry.viewResolver(viewResolver);
	}
	
	//配置静态资源读取器
	@Override
	public void addResourceHandlers(ResourceHandlerRegistry registry)
	{
		registry.addResourceHandler("/static/**").addResourceLocations("/static/");
	}
	
	//配置自定义转换器,用于转换string values[Roles] to UserProfiles in newUser.jsp
	@Override
	public void addFormatters(FormatterRegistry registry)
	{
		registry.addConverter(roleToUserProfileConverter);
	}
	
	//配置一个信息源，用于处理国际化资源信息, 注意这个Bean的名字一定要是messageSource
	//MessageSource是Spring定义的一个接口用于处理国际化信息
	//ResourceBundleMessageSource是MessageSource的实现类, 该实现类允许用户通过BeanName指定一个资源名(包括类路径的全限定
	//  资源名), 或通过beanName指定一组资源名
	@Bean
	public MessageSource messageSource()
	{
		ResourceBundleMessageSource messageSource = new ResourceBundleMessageSource();
		messageSource.setBasename("messages");
		return messageSource;
	}
	
	//配置定义URL配备规则
	//setUseSuffixPatternMatch : 设置是否是后缀模式匹配，如“/user”是否匹配/user.*，默认真即匹配
	//setUseTrailingSlashMatch : 设置是否自动后缀路径模式匹配，如“/user”是否匹配“/user/”，默认真即匹配
	@Override
	public void configurePathMatch(PathMatchConfigurer matcher)
	{
		matcher.setUseRegisteredSuffixPatternMatch(true);
	}
	
}
