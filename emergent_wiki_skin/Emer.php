<?php
/**
 * Emer - Branch of MonoBook which has many usability improvements and
 * somewhat cleaner code.
 *
 * @todo document
 * @file
 * @ingroup Skins
 */

if( !defined( 'MEDIAWIKI' ) )
	die( -1 );

/**
 * SkinTemplate class for Emer skin
 * @ingroup Skins
 */
class SkinEmer extends SkinTemplate {

	/* Functions */
	var $skinname = 'emer', $stylename = 'emer',
		$template = 'EmerTemplate';

	/**
	 * Initializes output page and sets up skin-specific parameters
	 * @param object $out Output page object to initialize
	 */
	public function initPage( OutputPage $out ) {
		global $wgStylePath, $wgJsMimeType, $wgStyleVersion;

		parent::initPage( $out );

		// Append skin-specific styles
		$out->addStyle( 'emer/main-rtl.css', 'screen', '', 'rtl' );
		$out->addStyle( 'emer/main-ltr.css', 'screen', '', 'ltr' );
		// Append CSS which includes IE only behavior fixes for hover support -
		// this is better than including this in a CSS fille since it doesn't
		// wait for the CSS file to load before fetching the HTC file.
		$out->addScript(
			'<!--[if lt IE 7]><style type="text/css">body{behavior:url("' .
				$wgStylePath .
				'/emer/csshover.htc")}</style><![endif]-->'
		);
		// Append common IE fixes, which perhaps should be included in all
		// skins, but for now it seems each skin needs to include them
		// explicitly
		$out->addScript(
			'<!--[if lt IE 7]><script type="' . $wgJsMimeType . '" src="' .
				$wgStylePath .
				'/common/IEFixes.js?' .
				$wgStyleVersion .
				'"></script>' .
				'<meta http-equiv="imagetoolbar" content="no" /><![endif]-->'
		);
	}
	/**
	 * Builds a structured array of links used for tabs and menus
	 * @return array
	 * @private
	 */
	function buildNavigationUrls() {
		global $wgContLang, $wgLang, $wgOut, $wgUser, $wgRequest, $wgArticle;
		global $wgDisableLangConversion;

		wfProfileIn( __METHOD__ );

		$links = array(
			'namespaces' => array(),
			'views' => array(),
			'actions' => array(),
			'variants' => array()
		);

		// Detects parameters
		$action = $wgRequest->getVal( 'action', 'view' );
		$section = $wgRequest->getVal( 'section' );

		// Checks if page is some kind of content
		if( $this->iscontent ) {

			// Gets page objects for the related namespaces
			$subjectPage = $this->mTitle->getSubjectPage();
			$talkPage = $this->mTitle->getTalkPage();

			// Determines if this is a talk page
			$isTalk = $this->mTitle->isTalkPage();

			// Generates XML IDs from namespace names
			$subjectId = $this->mTitle->getNamespaceKey( '' );

			if ( $subjectId == 'main' ) {
				$talkId = 'talk';
			} else {
				$talkId = "{$subjectId}_talk";
			}
			$currentId = $isTalk ? $talkId : $subjectId;

			// Adds namespace links
			$links['namespaces'][$subjectId] = $this->tabAction(
				$subjectPage, 'vector-namespace-' . $subjectId, !$isTalk, '', true
			);
			$links['namespaces'][$subjectId]['context'] = 'subject';
			$links['namespaces'][$talkId] = $this->tabAction(
				$talkPage, 'vector-namespace-talk', $isTalk, '', true
			);
			$links['namespaces'][$talkId]['context'] = 'talk';

			// Adds view view link
			if ( $this->mTitle->exists() ) {
				$links['views']['view'] = $this->tabAction(
					$isTalk ? $talkPage : $subjectPage,
						'vector-view-view', ( $action == 'view' ), '', true
				);
			}

			wfProfileIn( __METHOD__ . '-edit' );

			// Checks if user can...
			if (
				// edit the current page
				$this->mTitle->quickUserCan( 'edit' ) &&
				(
					// if it exists
					$this->mTitle->exists() ||
					// or they can create one here
					$this->mTitle->quickUserCan( 'create' )
				)
			) {
				// Builds CSS class for talk page links
				$isTalkClass = $isTalk ? ' istalk' : '';

				// Determines if we're in edit mode
				$selected = (
					( $action == 'edit' || $action == 'submit' ) &&
					( $section != 'new' )
				);
				$links['views']['edit'] = array(
					'class' => ( $selected ? 'selected' : '' ) . $isTalkClass,
					'text' => $this->mTitle->exists()
						? wfMsg( 'vector-view-edit' )
						: wfMsg( 'vector-view-create' ),
					'href' =>
						$this->mTitle->getLocalUrl( $this->editUrlOptions() )
				);
				// Checks if this is a current rev of talk page and we should show a new
				// section link
				if ( ( $isTalk && $wgArticle->isCurrent() ) || ( $wgOut->showNewSectionLink() ) ) {
					// Checks if we should ever show a new section link
					if ( !$wgOut->forceHideNewSectionLink() ) {
						// Adds new section link
						//$links['actions']['addsection']
						$links['views']['addsection'] = array(
							'class' => $section == 'new' ? 'selected' : false,
							'text' => wfMsg( 'vector-action-addsection' ),
							'href' => $this->mTitle->getLocalUrl(
								'action=edit&section=new'
							)
						);
					}
				}
			// Checks if the page is known (some kind of viewable content)
			} elseif ( $this->mTitle->isKnown() ) {
				// Adds view source view link
				$links['views']['viewsource'] = array(
					'class' => ( $action == 'edit' ) ? 'selected' : false,
					'text' => wfMsg( 'vector-view-viewsource' ),
					'href' =>
						$this->mTitle->getLocalUrl( $this->editUrlOptions() )
				);
			}
			wfProfileOut( __METHOD__ . '-edit' );

			wfProfileIn( __METHOD__ . '-live' );

			// Checks if the page exists
			if ( $this->mTitle->exists() ) {
				// Adds history view link
				$links['views']['history'] = array(
					'class' => ($action == 'history') ? 'selected' : false,
					'text' => wfMsg( 'vector-view-history' ),
					'href' => $this->mTitle->getLocalUrl( 'action=history' ),
					'rel' => 'archives',
				);

				if( $wgUser->isAllowed( 'delete' ) ) {
					$links['actions']['delete'] = array(
						'class' => ($action == 'delete') ? 'selected' : false,
						'text' => wfMsg( 'vector-action-delete' ),
						'href' => $this->mTitle->getLocalUrl( 'action=delete' )
					);
				}
				if ( $this->mTitle->quickUserCan( 'move' ) ) {
					$moveTitle = SpecialPage::getTitleFor(
						'Movepage', $this->thispage
					);
					$links['actions']['move'] = array(
						'class' => $this->mTitle->isSpecial( 'Movepage' ) ?
										'selected' : false,
						'text' => wfMsg( 'vector-action-move' ),
						'href' => $moveTitle->getLocalUrl()
					);
				}

				if (
					$this->mTitle->getNamespace() !== NS_MEDIAWIKI &&
					$wgUser->isAllowed( 'protect' )
				) {
					if ( !$this->mTitle->isProtected() ){
						$links['actions']['protect'] = array(
							'class' => ($action == 'protect') ?
											'selected' : false,
							'text' => wfMsg( 'vector-action-protect' ),
							'href' =>
								$this->mTitle->getLocalUrl( 'action=protect' )
						);

					} else {
						$links['actions']['unprotect'] = array(
							'class' => ($action == 'unprotect') ?
											'selected' : false,
							'text' => wfMsg( 'vector-action-unprotect' ),
							'href' =>
								$this->mTitle->getLocalUrl( 'action=unprotect' )
						);
					}
				}
			} else {
				// article doesn't exist or is deleted
				if (
					$wgUser->isAllowed( 'deletedhistory' ) &&
					$wgUser->isAllowed( 'undelete' )
				) {
					if( $n = $this->mTitle->isDeleted() ) {
						$undelTitle = SpecialPage::getTitleFor( 'Undelete' );
						$links['actions']['undelete'] = array(
							'class' => false,
							'text' => wfMsgExt(
								'vector-action-undelete',
								array( 'parsemag' ),
								$wgLang->formatNum( $n )
							),
							'href' => $undelTitle->getLocalUrl(
								'target=' . urlencode( $this->thispage )
							)
						);
					}
				}

				if (
					$this->mTitle->getNamespace() !== NS_MEDIAWIKI &&
					$wgUser->isAllowed( 'protect' )
				) {
					if ( !$this->mTitle->getRestrictions( 'create' ) ) {
						$links['actions']['protect'] = array(
							'class' => ($action == 'protect') ?
											'selected' : false,
							'text' => wfMsg( 'vector-action-protect' ),
							'href' =>
								$this->mTitle->getLocalUrl( 'action=protect' )
						);

					} else {
						$links['actions']['unprotect'] = array(
							'class' => ($action == 'unprotect') ?
											'selected' : false,
							'text' => wfMsg( 'vector-action-unprotect' ),
							'href' =>
								$this->mTitle->getLocalUrl( 'action=unprotect' )
						);
					}
				}
			}
			wfProfileOut( __METHOD__ . '-live' );

			/**
			 * The following actions use messages which, if made particular to
			 * the Emer skin, would break the Ajax code which makes this
			 * action happen entirely inline. Skin::makeGlobalVariablesScript
			 * defines a set of messages in a javascript object - and these
			 * messages are assumed to be global for all skins. Without making
			 * a change to that procedure these messages will have to remain as
			 * the global versions.
			 */
			// Checks if the user is logged in
			if( $this->loggedin ) {
				// Checks if the user is watching this page
				if( !$this->mTitle->userIsWatching() ) {
					// Adds watch action link
					$links['actions']['watch'] = array(
						'class' =>
							( $action == 'watch' or $action == 'unwatch' ) ?
								'selected' : false,
						'text' => wfMsg( 'watch' ),
						'href' => $this->mTitle->getLocalUrl( 'action=watch' )
					);
				} else {
					// Adds unwatch action link
					$links['actions']['unwatch'] = array(
						'class' =>
							($action == 'unwatch' or $action == 'watch') ?
								'selected' : false,
						'text' => wfMsg( 'unwatch' ),
						'href' => $this->mTitle->getLocalUrl( 'action=unwatch' )
					);
				}
			}

			// This is instead of SkinTemplateTabs - which uses a flat array
			wfRunHooks( 'SkinTemplateNavigation', array( &$this, &$links ) );

		// If it's not content, it's got to be a special page
		} else {
			$links['namespaces']['special'] = array(
				'class' => 'selected',
				'text' => wfMsg( 'vector-namespace-special' ),
				'href' => $wgRequest->getRequestURL()
			);
		}

		// Gets list of language variants
		$variants = $wgContLang->getVariants();
		// Checks that language conversion is enabled and variants exist
		if( !$wgDisableLangConversion && count( $variants ) > 1 ) {
			// Gets preferred variant
			$preferred = $wgContLang->getPreferredVariant();
			// Loops over each variant
			foreach( $variants as $code ) {
				// Gets variant name from language code
				$varname = $wgContLang->getVariantname( $code );
				// Checks if the variant is marked as disabled
				if( $varname == 'disable' ) {
					// Skips this variant
					continue;
				}
				// Appends variant link
				$links['variants'][] = array(
					'class' => ( $code == $preferred ) ? 'selected' : false,
					'text' => $varname,
					'href' => $this->mTitle->getLocalURL( '', $code )
				);
			}
		}

		wfProfileOut( __METHOD__ );

		return $links;
	}
}

/**
 * QuickTemplate class for Emer skin
 * @ingroup Skins
 */
class EmerTemplate extends QuickTemplate {

	/* Members */

	/**
	 * @var Cached skin object
	 */
	var $skin;

	/* Functions */

	/**
	 * Outputs the entire contents of the XHTML page
	 */
	public function execute() {
		global $wgRequest, $wgOut, $wgContLang;

		$this->skin = $this->data['skin'];
		$action = $wgRequest->getText( 'action' );

		// Build additional attributes for navigation urls
		$nav = $this->skin->buildNavigationUrls();
		foreach ( $nav as $section => $links ) {
			foreach ( $links as $key => $link ) {
				$xmlID = $key;
				if ( isset( $link['context'] ) && $link['context'] == 'subject' ) {
					$xmlID = 'ca-nstab-' . $xmlID;
				} else if ( isset( $link['context'] ) && $link['context'] == 'talk' ) {
					$xmlID = 'ca-talk';
				} else {
					$xmlID = 'ca-' . $xmlID;
				}
				$nav[$section][$key]['attributes'] =
					' id="' . Sanitizer::escapeId( $xmlID ) . '"';
			 	if ( $nav[$section][$key]['class'] ) {
					$nav[$section][$key]['attributes'] .=
						' class="' . htmlspecialchars( $link['class'] ) . '"';
					unset( $nav[$section][$key]['class'] );
			 	}
				// We don't want to give the watch tab an accesskey if the page
				// is being edited, because that conflicts with the accesskey on
				// the watch checkbox.  We also don't want to give the edit tab
				// an accesskey, because that's fairly superfluous and conflicts
				// with an accesskey (Ctrl-E) often used for editing in Safari.
			 	if (
					in_array( $action, array( 'edit', 'submit' ) ) &&
					in_array( $key, array( 'edit', 'watch', 'unwatch' ) )
				) {
			 		$nav[$section][$key]['key'] =
						$this->skin->tooltip( $xmlID );
			 	} else {
			 		$nav[$section][$key]['key'] =
						$this->skin->tooltipAndAccesskey( $xmlID );
			 	}
			}
		}
		$this->data['namespace_urls'] = $nav['namespaces'];
		$this->data['view_urls'] = $nav['views'];
		$this->data['action_urls'] = $nav['actions'];
		$this->data['variant_urls'] = $nav['variants'];
		// Build additional attributes for personal_urls
		foreach ( $this->data['personal_urls'] as $key => $item) {
			$this->data['personal_urls'][$key]['attributes'] =
				' id="' . Sanitizer::escapeId( "pt-$key" ) . '"';
			if ( isset( $item['active'] ) && $item['active'] ) {
				$this->data['personal_urls'][$key]['attributes'] .=
					' class="active"';
			}
			$this->data['personal_urls'][$key]['key'] =
				$this->skin->tooltipAndAccesskey('pt-'.$key);
		}

		// Generate additional footer links
		$footerlinks = array(
			'info' => array(
				'lastmod',
				'viewcount',
				'numberofwatchingusers',
				'credits',
				'copyright',
				'tagline',
			),
			'places' => array(
				'privacy',
				'about',
				'disclaimer',
			),
		);
		// Reduce footer links down to only those which are being used
		$validFooterLinks = array();
		foreach( $footerlinks as $category => $links ) {
			$validFooterLinks[$category] = array();
			foreach( $links as $link ) {
				if( isset( $this->data[$link] ) && $this->data[$link] ) {
					$validFooterLinks[$category][] = $link;
				}
			}
		}
		// Reverse horizontally rendered navigation elements
		if ( $wgContLang->isRTL() ) {
			$this->data['view_urls'] =
				array_reverse( $this->data['view_urls'] );
			$this->data['namespace_urls'] =
				array_reverse( $this->data['namespace_urls'] );
			$this->data['personal_urls'] =
				array_reverse( $this->data['personal_urls'] );
		}
		// Output HTML Page
		echo $wgOut->headElement( $this->skin );
?>
	<body<?php if ( $this->data['body_ondblclick'] ): ?> ondblclick="<?php $this->text( 'body_ondblclick' ) ?>"<?php endif; ?> <?php if ( $this->data['body_onload'] ): ?> onload="<?php $this->text( 'body_onload' ) ?>"<?php endif; ?> class="mediawiki <?php $this->text( 'dir' ) ?> <?php $this->text( 'pageclass' ) ?> <?php $this->text( 'skinnameclass' ) ?>" dir="<?php $this->text( 'dir' ) ?>">
                <div style="position:absolute; top:5px; left: 5px;z-index:99999;">
		     <a href="http://grey.colorado.edu/emergent">
		     	<img width="30px" src="http://grey.colorado.edu/mediawiki/sites/emergent/logo.png"><span style="font-size:150%">mergent</span>
		     </a>
		</div>
		<div id="page-base" class="noprint"></div>
		<div id="head-base" class="noprint"></div>
		<!-- content -->
		<div id="content">
			<a id="top"></a>
			<div id="mw-js-message" style="display:none;"></div>
			<?php if ( $this->data['sitenotice'] ): ?>
			<!-- sitenotice -->
			<div id="siteNotice"><?php $this->html( 'sitenotice' ) ?></div>
			<!-- /sitenotice -->
			<?php endif; ?>
			<!-- firstHeading -->
			<h1 id="firstHeading" class="firstHeading"><?php $this->html( 'title' ) ?></h1>
			<!-- /firstHeading -->
			<!-- bodyContent -->
			<div id="bodyContent">

				<!-- tagline -->
				<h3 id="siteSub"><?php $this->msg( 'tagline' ) ?></h3>
				<!-- /tagline -->
				<!-- subtitle -->
				<div id="contentSub"><?php $this->html( 'subtitle' ) ?></div>
				<!-- /subtitle -->
				<?php if ( $this->data['undelete'] ): ?>
				<!-- undelete -->
				<div id="contentSub2"><?php $this->html( 'undelete' ) ?></div>
				<!-- /undelete -->
				<?php endif; ?>
				<?php if($this->data['newtalk'] ): ?>
				<!-- newtalk -->
				<div class="usermessage"><?php $this->html( 'newtalk' )  ?></div>
				<!-- /newtalk -->
				<?php endif; ?>
				<?php if ( $this->data['showjumplinks'] ): ?>
				<!-- jumpto -->
				<div id="jump-to-nav">
					<?php $this->msg( 'jumpto' ) ?><a href="#head"><?php $this->msg( 'jumptonavigation' ) ?></a>,
					<a href="#p-search"><?php $this->msg( 'jumptosearch' ) ?></a>
				</div>
				<!-- /jumpto -->
				<?php endif; ?>
				<!-- bodytext -->
				<?php $this->html( 'bodytext' ) ?>
				<!-- /bodytext -->
				<?php if ( $this->data['catlinks'] ): ?>
				<!-- catlinks -->
				<?php $this->html( 'catlinks' ); ?>
				<!-- /catlinks -->
				<?php endif; ?>
				<?php if ( $this->data['dataAfterContent'] ): ?>
				<!-- dataAfterContent -->
				<?php $this->html( 'dataAfterContent' ); ?>
				<!-- /dataAfterContent -->
				<?php endif; ?>
				<div class="visualClear"></div>
			</div>
			<!-- /bodyContent -->
		</div>
		<!-- /content -->
		<!-- header -->
		<div id="head" class="noprint">
			<?php $this->renderNavigation( 'PERSONAL' ); ?>
			<div id="left-navigation">
				<?php $this->renderNavigation( array( 'NAMESPACES', 'VARIANTS' ) ); ?>
			</div>
			<div id="right-navigation">
				<?php $this->renderNavigation( array( 'VIEWS', 'ACTIONS', 'SEARCH' ) ); ?>
			</div>
		</div>
		<!-- /header -->
		<!-- footer -->
		<div id="footer">
			<?php foreach( $validFooterLinks as $category => $links ): ?>
				<?php if ( count( $links ) > 0 ): ?>
				<ul id="footer-<?php echo $category ?>">
					<?php foreach( $links as $link ): ?>
						<?php if( isset( $this->data[$link] ) && $this->data[$link] ): ?>
						<li id="footer-<?php echo $category ?>-<?php echo $link ?>"><?php $this->html( $link ) ?></li>
						<?php endif; ?>
					<?php endforeach; ?>
				</ul>
				<?php endif; ?>
			<?php endforeach; ?>
			<ul id="footer-icons" class="noprint">
				<?php if ( $this->data['poweredbyico'] ): ?>
				<li id="footer-icon-poweredby"><?php $this->html( 'poweredbyico' ) ?></li>
				<?php endif; ?>
				<?php if ( $this->data['copyrightico'] ): ?>
				<li id="footer-icon-copyright"><?php $this->html( 'copyrightico' ) ?></li>
				<?php endif; ?>
			</ul>
			<div style="clear:both"></div>
		</div>
		<!-- /footer -->
		<!-- fixalpha -->
		<script type="<?php $this->text('jsmimetype') ?>"> if ( window.isMSIE55 ) fixalpha(); </script>
		<!-- /fixalpha -->
		<?php $this->html( 'bottomscripts' ); /* JS call to runBodyOnloadHook */ ?>
		<?php $this->html( 'reporttime' ) ?>
		<?php if ( $this->data['debug'] ): ?>
		<!-- Debug output: <?php $this->text( 'debug' ); ?> -->
		<?php endif; ?>
	</body>
</html>
<?php
	}

	/**
	 * Render a series of portals
	 */
	private function renderPortals( $portals ) {
		// Force the rendering of the following portals
		if ( !isset( $portals['SEARCH'] ) ) $portals['SEARCH'] = true;
		if ( !isset( $portals['TOOLBOX'] ) ) $portals['TOOLBOX'] = true;
		if ( !isset( $portals['LANGUAGES'] ) ) $portals['LANGUAGES'] = true;
		// Render portals
		foreach ( $portals as $name => $content ) {
			echo "\n<!-- {$name} -->\n";
			switch( $name ) {
				case 'SEARCH':
					break;
				case 'TOOLBOX':
?>
		<?php if( $this->data['notspecialpage'] ): ?>
			<li id="t-whatlinkshere"><a href="<?php echo htmlspecialchars( $this->data['nav_urls']['whatlinkshere']['href'] ) ?>"<?php echo $this->skin->tooltipAndAccesskey( 't-whatlinkshere' ) ?>><?php $this->msg( 'whatlinkshere' ) ?></a></li>
			<?php if( $this->data['nav_urls']['recentchangeslinked'] ): ?>
			<li id="t-recentchangeslinked"><a href="<?php echo htmlspecialchars( $this->data['nav_urls']['recentchangeslinked']['href'] ) ?>"<?php echo $this->skin->tooltipAndAccesskey( 't-recentchangeslinked' ) ?>><?php $this->msg( 'recentchangeslinked-toolbox' ) ?></a></li>
			<?php endif; ?>
		<?php endif; ?>
		<?php if( isset( $this->data['nav_urls']['trackbacklink'] ) ): ?>
		<li id="t-trackbacklink"><a href="<?php echo htmlspecialchars( $this->data['nav_urls']['trackbacklink']['href'] ) ?>"<?php echo $this->skin->tooltipAndAccesskey( 't-trackbacklink' ) ?>><?php $this->msg( 'trackbacklink' ) ?></a></li>
		<?php endif; ?>
		<?php if( $this->data['feeds']): ?>
		<li id="feedlinks">
			<?php foreach( $this->data['feeds'] as $key => $feed ): ?>
			<a id="<?php echo Sanitizer::escapeId( "feed-$key" ) ?>" href="<?php echo htmlspecialchars( $feed['href'] ) ?>" rel="alternate" type="application/<?php echo $key ?>+xml" class="feedlink"<?php echo $this->skin->tooltipAndAccesskey( 'feed-' . $key ) ?>><?php echo htmlspecialchars( $feed['text'] ) ?></a>
			<?php endforeach; ?>
		</li>
		<?php endif; ?>
		<?php foreach( array( 'contributions', 'log', 'blockip', 'emailuser', 'upload', 'specialpages' ) as $special ): ?>
			<?php if( $this->data['nav_urls'][$special]): ?>
			<li id="t-<?php echo $special ?>"><a href="<?php echo htmlspecialchars( $this->data['nav_urls'][$special]['href'] ) ?>"<?php echo $this->skin->tooltipAndAccesskey( 't-' . $special ) ?>><?php $this->msg( $special ) ?></a></li>
			<?php endif; ?>
		<?php endforeach; ?>
		<?php if( !empty( $this->data['nav_urls']['print']['href'] ) ): ?>
		<li id="t-print"><a href="<?php echo htmlspecialchars( $this->data['nav_urls']['print']['href'] ) ?>" rel="alternate"<?php echo $this->skin->tooltipAndAccesskey( 't-print' ) ?>><?php $this->msg( 'printableversion' ) ?></a></li>
		<?php endif; ?>
		<?php if (  !empty(  $this->data['nav_urls']['permalink']['href'] ) ): ?>
		<li id="t-permalink"><a href="<?php echo htmlspecialchars( $this->data['nav_urls']['permalink']['href'] ) ?>"<?php echo $this->skin->tooltipAndAccesskey( 't-permalink' ) ?>><?php $this->msg( 'permalink' ) ?></a></li>
		<?php elseif ( $this->data['nav_urls']['permalink']['href'] === '' ): ?>
		<li id="t-ispermalink"<?php echo $this->skin->tooltip( 't-ispermalink' ) ?>><?php $this->msg( 'permalink' ) ?></li>
		<?php endif; ?>
<?php
					break;
				case 'LANGUAGES':
					if ( $this->data['language_urls'] ) {
?>
<div class="portal" id="p-lang">
	<h5 <?php $this->html('userlangattributes') ?>><?php $this->msg( 'otherlanguages' ) ?></h5>
	<div class="body">
		<ul>
		<?php foreach ( $this->data['language_urls'] as $langlink ): ?>
			<li class="<?php echo htmlspecialchars(  $langlink['class'] ) ?>"><a href="<?php echo htmlspecialchars( $langlink['href'] ) ?>"><?php echo $langlink['text'] ?></a></li>
		<?php endforeach; ?>
		</ul>
	</div>
</div>
<?php
					}
					break;
				case 'navigation':
?>
		<?php foreach( $content as $key => $val ): ?>
			<li id="<?php echo Sanitizer::escapeId( $val['id'] ) ?>"<?php if ( $val['active'] ): ?> class="active" <?php endif; ?>><a href="<?php echo htmlspecialchars( $val['href'] ) ?>"<?php echo $this->skin->tooltipAndAccesskey( $val['id'] ) ?>><?php echo htmlspecialchars( $val['text'] ) ?></a></li>
		<?php endforeach; ?>
<?php
					break;
				default:
?>
<div class="portal" id='<?php echo Sanitizer::escapeId( "p-$name" ) ?>'<?php echo $this->skin->tooltip( 'p-' . $name ) ?>>
	<h5 <?php $this->html('userlangattributes') ?>><?php $out = wfMsg( $name ); if ( wfEmptyMsg( $name, $out ) ) echo htmlspecialchars( $name ); else echo htmlspecialchars( $out ); ?></h5>
	<div class="body">
		<?php if ( is_array( $content ) ): ?>
		<ul>
		<?php foreach( $content as $key => $val ): ?>
			<li id="<?php echo Sanitizer::escapeId( $val['id'] ) ?>"<?php if ( $val['active'] ): ?> class="active" <?php endif; ?>><a href="<?php echo htmlspecialchars( $val['href'] ) ?>"<?php echo $this->skin->tooltipAndAccesskey( $val['id'] ) ?>><?php echo htmlspecialchars( $val['text'] ) ?></a></li>
		<?php endforeach; ?>
		</ul>
		<?php else: ?>
		<?php echo $content; /* Allow raw HTML block to be defined by extensions */ ?>
		<?php endif; ?>
	</div>
</div>
<?php
				break;
			}
			echo "\n<!-- /{$name} -->\n";
		}
	}

	/**
	 * Render one or more navigations elements by name, automatically reveresed
	 * when UI is in RTL mode
	 */
 	private function renderNavigation( $elements ) {
		global $wgContLang, $wgEmerUseSimpleSearch, $wgStylePath;

		// If only one element was given, wrap it in an array, allowing more
		// flexible arguments
		if ( !is_array( $elements ) ) {
			$elements = array( $elements );
		// If there's a series of elements, reverse them when in RTL mode
		} else if ( $wgContLang->isRTL() ) {
			$elements = array_reverse( $elements );
		}
		// Render elements
		foreach ( $elements as $name => $element ) {
			echo "\n<!-- {$name} -->\n";
			switch ( $element ) {
				case 'NAMESPACES':
?>
<div id="p-namespaces" class="emerTabs<?php if ( count( $this->data['namespace_urls'] ) == 0 ) echo ' emptyPortlet'; ?>">
	<h5><?php $this->msg('namespaces') ?></h5>
	<ul <?php $this->html('userlangattributes') ?>>
		<?php foreach ($this->data['namespace_urls'] as $key => $link ): ?>
			<li <?php echo $link['attributes'] ?>><a href="<?php echo htmlspecialchars( $link['href'] ) ?>" <?php echo $link['key'] ?>><span><?php echo htmlspecialchars( $link['text'] ) ?></span></a></li>
		<?php endforeach; ?>
	</ul>
</div>
<?php
				break;
				case 'VARIANTS':
?>
<div id="p-variants" class="emerMenu<?php if ( count( $this->data['variant_urls'] ) == 0 ) echo ' emptyPortlet'; ?>">
	<h5><span><?php $this->msg('variants') ?></span><a href="#"></a></h5>
	<div class="menu">
		<ul <?php $this->html('userlangattributes') ?>>
			<?php foreach ($this->data['variant_urls'] as $key => $link ): ?>
				<li<?php echo $link['attributes'] ?>><a href="<?php echo htmlspecialchars( $link['href'] ) ?>" <?php echo $link['key'] ?>><?php echo htmlspecialchars( $link['text'] ) ?></a></li>
			<?php endforeach; ?>
		</ul>
	</div>
</div>
<?php
				break;
				case 'VIEWS':
?>
<div id="p-views" class="emerTabs<?php if ( count( $this->data['view_urls'] ) == 0 ) echo ' emptyPortlet'; ?>">

	<h5><?php $this->msg('views') ?></h5>
	<ul <?php $this->html('userlangattributes') ?>>
		<?php foreach ($this->data['view_urls'] as $key => $link ): ?>
			<li<?php echo $link['attributes'] ?>><a href="<?php echo htmlspecialchars( $link['href'] ) ?>" <?php echo $link['key'] ?>><span><?php echo htmlspecialchars( $link['text'] ) ?></span></a></li>
		<?php endforeach; ?>
	</ul>
</div>
<?php
				break;
				case 'ACTIONS':
?>
<div id="p-cactions" class="emerMenu">
	<h5><span><?php $this->msg('actions') ?></span><a href="#"></a></h5>
	<div class="menu">
	     <ul>

			<?php foreach ($this->data['action_urls'] as $key => $link ): ?>
				<li<?php echo $link['attributes'] ?>><a href="<?php echo htmlspecialchars( $link['href'] ) ?>" <?php echo $link['key'] ?>><?php echo htmlspecialchars( $link['text'] ) ?></a></li>
			<?php endforeach; ?>
			<?php $this->renderPortals( $this->data['sidebar'] ); ?>
	     </ul>
	</div>
</div>
<?php
				break;
				case 'PERSONAL':
?>
<div id="p-personal" class="<?php if ( count( $this->data['personal_urls'] ) == 0 ) echo ' emptyPortlet'; ?>">
	<h5><?php $this->msg('personaltools') ?></h5>
	<ul <?php $this->html('userlangattributes') ?>>
		<?php foreach($this->data['personal_urls'] as $key => $item): ?>
			<li <?php echo $item['attributes'] ?>><a href="<?php echo htmlspecialchars($item['href']) ?>"<?php echo $item['key'] ?><?php if(!empty($item['class'])): ?> class="<?php echo htmlspecialchars($item['class']) ?>"<?php endif; ?>><?php echo htmlspecialchars($item['text']) ?></a></li>
		<?php endforeach; ?>
	</ul>
</div>
<?php
				break;
				case 'SEARCH':
?>
<div id="p-search">
<div id="cse">Loading</div><script src="http://www.google.com/jsapi" type="text/javascript"></script><script type="text/javascript">google.load('search', '1');google.setOnLoadCallback(function(){new google.search.CustomSearchControl('001223136417691641822:3aawef4fybc').draw('cse');}, true);</script>
</div>
</div>
<?php

				break;
			}
			echo "\n<!-- /{$name} -->\n";
		}
	}
}
