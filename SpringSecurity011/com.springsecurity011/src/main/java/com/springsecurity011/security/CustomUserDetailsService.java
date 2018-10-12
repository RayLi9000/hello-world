package com.springsecurity011.security;

import java.util.ArrayList;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.stereotype.Service;

import com.springsecurity011.model.User;
import com.springsecurity011.model.UserProfile;
import com.springsecurity011.service.UserService;

@Service("customUserDetailsService")
public class CustomUserDetailsService implements UserDetailsService
{
	@Autowired
	private UserService userService;
	
	static final Logger logger = LoggerFactory.getLogger(CustomUserDetailsService.class);
	
	@Override
	public UserDetails loadUserByUsername(String sso) throws UsernameNotFoundException 
	{
		User user = userService.findBySSO(sso);
		logger.info("User : {}", user);
		if (user == null)
		{
			logger.info("User not found");
			throw new UsernameNotFoundException("Username not found");
		}
		return new org.springframework.security.core.userdetails.User(user.getSsoId(), user.getPassword(), true,
																	true, true, true, getGrantedAuthorities(user));
	}
	
	
	private List<GrantedAuthority> getGrantedAuthorities(User user)
	{
		List<GrantedAuthority> authorities = new ArrayList<GrantedAuthority>();
		for (UserProfile profile : user.getUserProfiles())
		{
			logger.info("UserProfile : {}", profile);
			authorities.add(new SimpleGrantedAuthority("ROLE_" + profile.getType()));
		}
		logger.info("Authorities : {}" + authorities);
		return authorities;
	}

}
