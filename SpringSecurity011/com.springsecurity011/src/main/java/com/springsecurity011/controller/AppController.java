package com.springsecurity011.controller;

import java.util.List;
import java.util.Locale;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.validation.Valid;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.MessageSource;
import org.springframework.security.authentication.AuthenticationTrustResolver;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.web.authentication.rememberme.PersistentTokenBasedRememberMeServices;
import org.springframework.stereotype.Controller;
import org.springframework.ui.ModelMap;
import org.springframework.validation.BindingResult;
import org.springframework.validation.FieldError;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.SessionAttributes;

import com.springsecurity011.model.User;
import com.springsecurity011.model.UserProfile;
import com.springsecurity011.service.UserProfileService;
import com.springsecurity011.service.UserService;

/**
 * 因为ModelMap上的属性值是在本次request中有效，一旦请求结束，或者服务器重定向的话，ModelMap上的属性就会笨哦清空，
 * 那么就不能实现跨请求的信息共享
 * 但是@SessionAttribue可以解决问题，就是将放到ModelMap上的属相也放到Session中，只要浏览器不关闭，Session就永远有效，
 * 这样子就可以实现跨请求的信息共享
 * 可以通过SessionStatus.setComplete()来删除Session中保存的参数
 * 同时SessionAttribute也可以讲保存的值注入到@ModelAttribute中，前提是他们都是名字一样
 *
 */

@Controller
@RequestMapping("/")
@SessionAttributes("roles")
public class AppController 
{
	@Autowired
	private UserService userService;
	
	@Autowired
	private UserProfileService userProfileService;
	
	@Autowired
	private MessageSource messageSource;
	
	@Autowired
	PersistentTokenBasedRememberMeServices persistentTokenBasedRememberMeService;
	
	@Autowired
	AuthenticationTrustResolver authenticationTrustResolver;
	
	@RequestMapping(value = {"/", "/list"}, method = RequestMethod.GET)
	public String listUsers(ModelMap model)
	{
		List<User> users = userService.findAllUsers();
		model.addAttribute("users", users);
		model.addAttribute("loggedinuser", getPrincipal());
		return "userslist";
	}
	
	@RequestMapping(value = {"/newuser"}, method = RequestMethod.GET)
	public String nweUSer(ModelMap model)
	{
		User user = new User();
		model.addAttribute("user", user);
		model.addAttribute("edit", false);
		model.addAttribute("loggedinuser", getPrincipal());
		System.out.println("********* roles = " + model.get("roles").toString());
		return "registration";
	}
	
	@RequestMapping(value = {"/newuser"}, method = RequestMethod.POST)
	public String saveUser(@Valid User user, BindingResult result, ModelMap model)
	{
		if (result.hasErrors()) return "registration";
		if (!userService.isUserSSOUnique(user.getId(), user.getSsoId()))
		{
			FieldError ssoError = new FieldError("user", "ssoId", messageSource.getMessage("non.unique.ssoId", new String[]{user.getSsoId()}, Locale.getDefault()));
			result.addError(ssoError);
			return "registration";
		}
		userService.saveUser(user);
		model.addAttribute("success", "User " + user.getFirstName() + " " + user.getLastName() + " registered successfully");
		model.addAttribute("loggedinuser", getPrincipal());
		return "registrationsuccess";
	}
	
	@RequestMapping(value = {"edit-user-{ssoId}"}, method = RequestMethod.GET)
	public String editUser(@PathVariable String ssoId, ModelMap model)
	{
		User user = userService.findBySSO(ssoId);
		model.addAttribute("user", user);
		model.addAttribute("edit", true);
		model.addAttribute("loggedinuser", getPrincipal());
		return "registration";
	}
	
	@RequestMapping(value = {"/edit-user-{ssoId}"}, method = RequestMethod.POST)
	public String updateUser(@Valid User user, BindingResult result, ModelMap model, @PathVariable String ssoId)
	{
		if (result.hasErrors()) return "registration";
		if (!userService.isUserSSOUnique(user.getId(), user.getSsoId()))
		{
			FieldError ssoError = new FieldError("user", "ssoId", messageSource.getMessage("non.unique.ssoId", new String[]{user.getSsoId()}, Locale.getDefault()));
			result.addError(ssoError);
			return "registration";
		}
		userService.updateUser(user);
		model.addAttribute("success", "User " + user.getFirstName() + " " + user.getLastName() + "updated successfully");
		model.addAttribute("loggedinuser", getPrincipal());
		return "registrationsuccess";
	}
	
	@RequestMapping(value = {"/delete-user-{ssoId}"}, method = RequestMethod.GET)
	public String deleteUser(@PathVariable String ssoId)
	{
		userService.deleteUserBySSO(ssoId);
		return "redirect:/list";
	}
	
	//This method provide UserProfile list to view 
	@ModelAttribute("roles")
	public List<UserProfile> initializeProfiles()
	{
		List<UserProfile> userProfiles = userProfileService.findAll();
		return userProfiles;
	}
	
	@RequestMapping(value = {"/Access_Denied"}, method = RequestMethod.GET)
	public String accessDeniedPage(ModelMap model)
	{	
		model.addAttribute("loggedinuser", getPrincipal());
		return "accessDenied";
	}
	
	@RequestMapping(value = {"/login"}, method = RequestMethod.GET)
	public String loginPage()
	{
		if (isCurrentAuthenticationAnonymous())
			return "login";
		else
			return "redirect:/list";
	}
	
	@RequestMapping(value = {"/logout"}, method = RequestMethod.GET)
	public String logoutPage(HttpServletRequest request, HttpServletResponse response, ModelMap model)
	{
		Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
		if (authentication != null)
		{
			persistentTokenBasedRememberMeService.logout(request, response, authentication);
			SecurityContextHolder.getContext().setAuthentication(null);
		}
		return "redirect:/login?logout";
	}
	
	private String getPrincipal()
	{
		String userName = null;
		Object principal = SecurityContextHolder.getContext().getAuthentication().getPrincipal();
		if (principal instanceof UserDetails)
		{
			userName = ((UserDetails) principal).getUsername();
		}
		else
			userName = principal.toString();
		return userName;
	}
	
	private boolean isCurrentAuthenticationAnonymous()
	{
		final Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
		return authenticationTrustResolver.isAnonymous(authentication);
	}
}
