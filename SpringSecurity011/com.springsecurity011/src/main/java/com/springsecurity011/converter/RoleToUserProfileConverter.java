package com.springsecurity011.converter;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.convert.converter.Converter;
import org.springframework.stereotype.Component;

import com.springsecurity011.model.UserProfile;
import com.springsecurity011.service.UserProfileService;

/**
 * A converter class used in views to map id's to actual userProfile objects.
 */
@Component
public class RoleToUserProfileConverter implements Converter<Object, UserProfile>
{
	@Autowired
	private UserProfileService userProfileService;
	
	static final Logger logger = LoggerFactory.getLogger(RoleToUserProfileConverter.class);
	
	@Override
	public UserProfile convert(Object element) 
	{
		Integer id = Integer.parseInt((String)element);
		UserProfile userProfile = userProfileService.findById(id);
		logger.info("Profile  {}", userProfile);
		return userProfile;
	}
}
