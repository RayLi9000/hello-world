package com.springsecurity011.security;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.authentication.AuthenticationTrustResolver;
import org.springframework.security.authentication.AuthenticationTrustResolverImpl;
import org.springframework.security.authentication.dao.DaoAuthenticationProvider;
import org.springframework.security.config.annotation.authentication.builders.AuthenticationManagerBuilder;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.annotation.web.configuration.WebSecurityConfigurerAdapter;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.authentication.rememberme.PersistentTokenBasedRememberMeServices;
import org.springframework.security.web.authentication.rememberme.PersistentTokenRepository;

/**
 * 认证流程 Request ---SpringSecurity(拦截) --> UserNamePasswordAuthentication--->(authenticate)--->AuthenticationManager---> 
 * 				AuthenticationProvider(DaoAuthenticationProvider)--->(调用)-->UserDetailsService--->(调用)loadUserByName() --> DB
 * 		其中我们可以替换成自己AuthenticationProvider和>UserDetailsService
 * 
 * 			DB-->UserDetails(封装)-->Principal-->(封装)---> Authentication-->(保存)--> SecurityContextHolder
 * 
 * 
 * 
 * RememberMe 流程
 * 		
 *  Request --SpringSecurityFilterChain(拦截)--> UsernamePasswordAuthenticationFilter -->(生成) Authentication--> AuthenticationManager -->
 *  AuthenticationProvider(DaoAuthenticationProvider --> (调用)-->UserDetailsService--->(调用)loadUserByName() --> DB 
 *  DB-->UserDetails(封装)-->Principal-->(封装)---> Authentication(这个是已经认证过的Authentication)-->认证成功后 --->
 *  											将认证过的Authentication(上面的Authentication)放进SecurityContext-->(然后)
 *  -->调用RememberService(PersistentTokenBasedRememberMeServices)的loginSuccess()方法 -->isRememberMeAuthenticated()? -->(Yes)
 *  -->调用onLoginSuccess()方法生成RememberMeToken-->调用tokenRepository.createNewToken(persistentToken)将RememberMeToken保存到数据库(PersistentLogins)
 *  , 同时调用addCookie()将RememeberMeToken写进cookie
 * 
 * 注意 persistent_logins名字字段要和默认的一致
 * 
 * 第二次登录流程
 * Request --> SpringSecurityFilterChain-->RememberMeAuthenticationFilter(doFilter)-->1.判断SecurityContext有没有
 * Authentication-->若没有--> 2.从cookie查询用户信息返回RememberMeAuthenticationToken-->调用authenticationManager.authenticate-->调用
 * RememberService(PersistentTokenBasedRememberMeServices)的getTokenForSeries()去数据库验证-->调用UserDetailsService()加载用户信息
 * -->最后返回一个新的Authentication，由SecurityContext()保存；
 * 
 *
 */


@Configuration
@EnableWebSecurity
public class SpringSecurityConfig extends WebSecurityConfigurerAdapter
{
	@Autowired
	@Qualifier("customUserDetailsService")
	private UserDetailsService userDetailsService; //注入一个自定义UserDetailsService
	
	@Autowired
	private PersistentTokenRepository tokenRepository; //注入一个自定义PersistentTokenRepository用于RememberMe功能
	
	//配置一个自定义的AuthenticationManager, 即是告诉SpringSecurity使用自定义的AuthenticationManager
	@Autowired
	public void configureGlobalSecurity(AuthenticationManagerBuilder auth) throws Exception
	{
		auth.userDetailsService(userDetailsService);
		auth.authenticationProvider(authenticationProvider());
	}
	
	@Override
	public void configure(HttpSecurity http) throws Exception
	{
		http.authorizeRequests()
			.antMatchers("/", "/list").access("hasRole('USER') or hasRole('ADMIN') or hasRole('DBA')")
			.antMatchers("/newUser/**", "delete-user-*").access("hasRole('ADMIN')")
			.antMatchers("/edit-user-*").access("hasRole('ADMIN') or hasRole('DBA')")
			.and().formLogin().loginPage("/login").loginProcessingUrl("/login").usernameParameter("ssoId").passwordParameter("password")
			.and().rememberMe().rememberMeParameter("remember-me").tokenRepository(tokenRepository).tokenValiditySeconds(86400)
			.and().csrf()
			.and().exceptionHandling().accessDeniedPage("/Access_Dnied");
	}
	
	@Bean
	public PasswordEncoder passwordEncoder()
	{
		return new BCryptPasswordEncoder();
	}
	
	//配置一个自定义的与数据库打交道的AuthenticationProvider
	@Bean
	public DaoAuthenticationProvider authenticationProvider()
	{
		DaoAuthenticationProvider daoAuthenticationProvider = new DaoAuthenticationProvider();
		daoAuthenticationProvider.setUserDetailsService(userDetailsService);
		daoAuthenticationProvider.setPasswordEncoder(passwordEncoder());
		return daoAuthenticationProvider;
	}
	
	//自定义一个PersistentTokenBasedRememberMeServices，当使用RememberMe时就会调用这个Service
	@Bean
	public PersistentTokenBasedRememberMeServices getPersistentTokenBasedRememberMeService()
	{
		PersistentTokenBasedRememberMeServices tokenBasedService = new PersistentTokenBasedRememberMeServices(
				"remember-me", userDetailsService, tokenRepository);
		return tokenBasedService;
		
	}
	
	/**
	 *  AuthenticationTrustResolver是一个接口，其中定义有两个方法，isAnonymous()和isRememberMe()，
	 *  它们都接收一个Authentication对象作为参数。它有一个默认实现类AuthenticationTrustResolverImpl，
	 *  Spring Security就是使用它来判断一个SecurityContextHolder持有的Authentication是否AnonymousAuthenticationToken或RememberMeAuthenticationToken。
	 *  如当ExceptionTranslationFilter捕获到一个AccessDecisionManager后就会使用它来判断当前Authentication对象是否为一个AnonymousAuthenticationToken，
	 *  如果是则交由AuthenticationEntryPoint处理，否则将返回403错误码。
	 */
	@Bean
	public AuthenticationTrustResolver getAuthenticationTrustResolver()
	{
		return new AuthenticationTrustResolverImpl();
	}
}
