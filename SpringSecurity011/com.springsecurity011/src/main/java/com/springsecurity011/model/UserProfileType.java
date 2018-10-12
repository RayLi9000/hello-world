package com.springsecurity011.model;

public enum UserProfileType 
{
	USER("USER"),
	ADMIN("ADMIN"),
	DBA("DBA");
	
	private String userProfileType;
	
	private UserProfileType(String userProfileType)
	{
		this.userProfileType = userProfileType;
	}
	
	public String getUserProfileType()
	{
		return userProfileType;
	}
}
